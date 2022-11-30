#include "EffectCompiler.h"
#include "ShaderEvents.h"
#include "ShaderCommands.h"
#include "EffectEvents.h"
#include "EngineErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "GraphicKernel/GraphicCommands.h"
#include "GraphicKernel/GraphicEvents.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;

EffectCompiler::EffectCompiler()
{
    m_hasMaterialProduced = false;

    m_onShaderProgramBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderProgramBuilt(e); });
    EventPublisher::Subscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onBuildProgramFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildProgramFailed(e); });
    EventPublisher::Subscribe(typeid(BuildShaderProgramFailed), m_onBuildProgramFailed);

    m_onSamplerStateCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnSamplerStateCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceSamplerStateCreated), m_onSamplerStateCreated);
    m_onBlendStateCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnBlendStateCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceAlphaBlendStateCreated), m_onBlendStateCreated);
    m_onDepthStateCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnDepthStateCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceDepthStencilStateCreated), m_onDepthStateCreated);
    m_onRasterizerStateCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnRasterizerStateCreated(e); });
    EventPublisher::Subscribe(typeid(DeviceRasterizerStateCreated), m_onRasterizerStateCreated);
}

EffectCompiler::~EffectCompiler()
{
    EventPublisher::Unsubscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onShaderProgramBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildShaderProgramFailed), m_onBuildProgramFailed);
    m_onBuildProgramFailed = nullptr;

    EventPublisher::Unsubscribe(typeid(DeviceSamplerStateCreated), m_onSamplerStateCreated);
    m_onSamplerStateCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(DeviceAlphaBlendStateCreated), m_onBlendStateCreated);
    m_onBlendStateCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(DeviceDepthStencilStateCreated), m_onDepthStateCreated);
    m_onDepthStateCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(DeviceRasterizerStateCreated), m_onRasterizerStateCreated);
    m_onRasterizerStateCreated = nullptr;
}

void EffectCompiler::CompileEffect(const EffectCompilingProfile& profile)
{
    m_profile = profile;
    m_builtPrograms.clear();
    m_builtPassStates.clear();
    m_builtEffectTechniques.clear();

    if (m_profile.m_techniques.empty())
    {
        EventPublisher::Post(std::make_shared<CompileEffectMaterialFailed>(m_profile.m_name, ErrorCode::compilingEmptyEffectTech));
        return;
    }
    m_builtEffectTechniques.reserve(m_profile.m_techniques.size());
    for (auto& tech : m_profile.m_techniques)
    {
        BuiltEffectTechniqueMeta built_effect_technique{ tech.m_name };
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            BuiltEffectPassMeta built_effect_pass{ pass.m_name };
            built_effect_technique.m_passes.emplace_back(built_effect_pass);
            if (m_builtPrograms.find(pass.m_program.m_programName) == m_builtPrograms.end())
            {
                m_builtPrograms.emplace(pass.m_program.m_programName, nullptr);
            }
            EffectPassStates built_pass_states;
            CommandBus::Post(std::make_shared<BuildShaderProgram>(pass.m_program));
            if (!pass.m_samplers.empty())
            {
                built_pass_states.m_samplers = std::vector<Graphics::IDeviceSamplerStatePtr>{};
                built_pass_states.m_samplerNames = std::vector<std::string>{};
            }
            for (auto& samp : pass.m_samplers)
            {
                CommandBus::Post(std::make_shared<CreateSamplerState>(samp.m_name, samp.m_data));
                built_pass_states.m_samplers->emplace_back(nullptr);
                built_pass_states.m_samplerNames->emplace_back(samp.m_variableName);
            }
            CommandBus::Post(std::make_shared<CreateDepthStencilState>(pass.m_depth.m_name, pass.m_depth.m_data));
            CommandBus::Post(std::make_shared<CreateBlendState>(pass.m_blend.m_name, pass.m_blend.m_data));
            CommandBus::Post(std::make_shared<CreateRasterizerState>(pass.m_rasterizer.m_name, pass.m_rasterizer.m_data));
            m_builtPassStates.emplace(pass.m_name, built_pass_states);
        }
        m_builtEffectTechniques.emplace_back(built_effect_technique);
    }
    m_hasMaterialProduced = false;
}

void EffectCompiler::OnShaderProgramBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_built = std::dynamic_pointer_cast<ShaderProgramBuilt, IEvent>(e);
    if (!ev_built) return;
    if (m_builtPrograms.find(ev_built->GetShaderName()) == m_builtPrograms.end()) return;
    m_builtPrograms[ev_built->GetShaderName()] = ev_built->GetProgram();
    TryBuildEffectPass(ev_built->GetShaderName());
}

void EffectCompiler::OnBuildProgramFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_fail = std::dynamic_pointer_cast<BuildShaderProgramFailed, IEvent>(e);
    if (!ev_fail) return;
    EventPublisher::Post(std::make_shared<CompileEffectMaterialFailed>(m_profile.m_name, ev_fail->GetErrorCode()));
}

void EffectCompiler::OnSamplerStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceSamplerStateCreated, IEvent>(e);
    if (!ev) return;
    auto pass_profiles = m_profile.FindPassesWithSamplerState(ev->GetStateName());
    if (pass_profiles.empty()) return;

    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        auto index = m_profile.FindSamplerIndexInPass(pass_name, ev->GetStateName());
        if (!index) return;
        auto it = m_builtPassStates.find(pass_name);
        if (it == m_builtPassStates.end()) return;
        it->second.m_samplers.value()[*index] = ev->GetState();
        TryBuildEffectPass(pass_name);
    }
}

void EffectCompiler::OnBlendStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceAlphaBlendStateCreated, IEvent>(e);
    if (!ev) return;
    auto pass_profiles = m_profile.FindPassesWithBlendState(ev->GetStateName());
    if (pass_profiles.empty()) return;

    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
        m_builtPassStates[pass_name].m_blend = ev->GetState();
        TryBuildEffectPass(pass_name);
    }
}

void EffectCompiler::OnDepthStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceDepthStencilStateCreated, IEvent>(e);
    if (!ev) return;
    auto pass_profiles = m_profile.FindPassesWithDepthState(ev->GetStateName());
    if (pass_profiles.empty()) return;

    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
        m_builtPassStates[pass_name].m_depth = ev->GetState();
        TryBuildEffectPass(pass_name);
    }
}

void EffectCompiler::OnRasterizerStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceRasterizerStateCreated, IEvent>(e);
    if (!ev) return;
    auto pass_profiles = m_profile.FindPassesWithRasterizerState(ev->GetStateName());
    if (pass_profiles.empty()) return;

    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
        m_builtPassStates[pass_name].m_rasterizer = ev->GetState();
        TryBuildEffectPass(pass_name);
    }
}

void EffectCompiler::TryBuildEffectPass(const std::string& program_name)
{
    if (m_builtPrograms.find(program_name) == m_builtPrograms.end()) return;
    if (!m_builtPrograms[program_name]) return;
    auto pass_profiles = m_profile.FindPassesWithProgram(program_name);
    if (pass_profiles.empty()) return;
    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        auto it = m_builtPassStates.find(pass_name);
        if (it == m_builtPassStates.end()) continue;
        if (it->second.m_samplers)
        {
            for (unsigned int i = 0; i < it->second.m_samplers.value().size(); i++)
            {
                if (it->second.m_samplers.value()[i] == nullptr) continue;
            }
        }
        if (!it->second.m_blend) continue;
        if (!it->second.m_depth) continue;
        if (!it->second.m_rasterizer) continue;

        std::string tech_name;
        for (auto& tech : m_builtEffectTechniques)
        {
            if (tech.m_passes.empty()) continue;
            for (auto& pass : tech.m_passes)
            {
                if (pass.m_name == pass_name)
                {
                    pass.m_pass = EffectPass(pass.m_name, m_builtPrograms[program_name], m_builtPassStates[pass_name]);
                    tech_name = tech.m_name;
                }
            }
        }
        TryBuildEffectTechniques(tech_name);
    }
}

void EffectCompiler::TryBuildEffectTechniques(const std::string& name)
{
    for (auto& built_tech : m_builtEffectTechniques)
    {
        if (built_tech.m_name != name) continue;
        if (!built_tech.HasAllPassBuilt()) return;

        for (auto& built_pass : built_tech.m_passes)
        {
            if (!built_pass.m_pass) return; // pass not yet built
        }
        built_tech.m_technique = EffectTechnique(built_tech.m_name, built_tech.RetrieveEffectPasses());
    }
    TryBuildEffectMaterial();
}

void EffectCompiler::TryBuildEffectMaterial()
{
    if (m_hasMaterialProduced) return;
    std::vector<EffectTechnique> effect_techniques;
    for (auto& built_tech : m_builtEffectTechniques)
    {
        if (!built_tech.m_technique) return;
        effect_techniques.emplace_back(built_tech.m_technique.value());
    }
    auto effect_material = std::make_unique<EffectMaterial>(m_profile.m_name, effect_techniques);
    EventPublisher::Post(std::make_shared<EffectMaterialCompiled>(m_profile.m_name, std::move(effect_material)));
    m_hasMaterialProduced = true;
}

//---------------------------------------------------------------------------------------------
bool EffectCompiler::BuiltEffectTechniqueMeta::HasAllPassBuilt()
{
    if (m_passes.empty()) return false;
    for (auto& built_pass : m_passes)
    {
        if (!built_pass.m_pass) return false;
    }
    return true;
}

std::vector<EffectPass> EffectCompiler::BuiltEffectTechniqueMeta::RetrieveEffectPasses()
{
    if (m_passes.empty()) return {};
    std::vector<EffectPass> effect_passes;
    for (auto& built_pass : m_passes)
    {
        effect_passes.emplace_back(built_pass.m_pass.value());
    }
    return effect_passes;
}

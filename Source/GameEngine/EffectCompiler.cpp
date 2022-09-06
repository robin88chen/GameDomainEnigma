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
    m_onShaderProgramBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderProgramBuilt(e); });
    EventPublisher::Subscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onProgramBuildFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnProgramBuildFailed(e); });
    EventPublisher::Subscribe(typeid(ShaderProgramBuildFailed), m_onProgramBuildFailed);

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
    EventPublisher::Unsubscribe(typeid(ShaderProgramBuildFailed), m_onProgramBuildFailed);
    m_onProgramBuildFailed = nullptr;

    EventPublisher::Unsubscribe(typeid(DeviceSamplerStateCreated), m_onSamplerStateCreated);
    m_onSamplerStateCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(DeviceAlphaBlendStateCreated), m_onBlendStateCreated);
    m_onBlendStateCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(DeviceDepthStencilStateCreated), m_onDepthStateCreated);
    m_onDepthStateCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(DeviceRasterizerStateCreated), m_onRasterizerStateCreated);
    m_onRasterizerStateCreated = nullptr;
}

void EffectCompiler::CompileEffect(const EffectCompilingPolicy& policy)
{
    m_policy = policy;
    m_builtPrograms.clear();
    m_builtPassStates.clear();
    m_builtEffectTechniques.clear();

    if (m_policy.m_techniques.empty())
    {
        EventPublisher::Post(std::make_shared<EffectMaterialCompileFailed>(m_policy.m_name, ErrorCode::compilingEmptyEffectTech));
        return;
    }
    m_builtEffectTechniques.reserve(m_policy.m_techniques.size());
    for (auto& tech : m_policy.m_techniques)
    {
        BuiltEffectTechniqueMeta built_effect_technique{ tech.m_name };
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            BuiltEffectPassMeta built_effect_pass{ pass.m_program.m_programName };
            built_effect_technique.m_passes.emplace_back(built_effect_pass);
            m_builtPrograms.emplace(pass.m_program.m_programName, nullptr);
            EffectPassStates built_pass_states;
            CommandBus::Post(std::make_shared<BuildShaderProgram>(pass.m_program));
            if (!pass.m_samplers.empty())
            {
                built_pass_states.m_samplers = {};
                built_pass_states.m_samplerNames = {};
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
            m_builtPassStates.emplace(pass.m_program.m_programName, built_pass_states);
        }
        m_builtEffectTechniques.emplace_back(built_effect_technique);
    }
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

void EffectCompiler::OnProgramBuildFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_fail = std::dynamic_pointer_cast<ShaderProgramBuildFailed, IEvent>(e);
    if (!ev_fail) return;
    EventPublisher::Post(std::make_shared<EffectMaterialCompileFailed>(m_policy.m_name, ev_fail->GetErrorCode()));
}

void EffectCompiler::OnSamplerStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceSamplerStateCreated, IEvent>(e);
    if (!ev) return;
    auto pass = m_policy.FindPassWithSamplerState(ev->GetStateName());
    if (!pass) return;
    auto index = m_policy.FindSamplerIndexInPass(ev->GetStateName());
    if (!index) return;
    std::string pass_name = pass.value().get().m_program.m_programName;
    auto it = m_builtPassStates.find(pass_name);
    if (it == m_builtPassStates.end()) return;
    it->second.m_samplers.value()[*index] = ev->GetState();
    TryBuildEffectPass(pass_name);
}

void EffectCompiler::OnBlendStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceAlphaBlendStateCreated, IEvent>(e);
    if (!ev) return;
    auto pass = m_policy.FindPassWithBlendState(ev->GetStateName());
    if (!pass) return;
    std::string pass_name = pass.value().get().m_program.m_programName;
    if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
    m_builtPassStates[pass_name].m_blend = ev->GetState();
    TryBuildEffectPass(pass_name);
}

void EffectCompiler::OnDepthStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceDepthStencilStateCreated, IEvent>(e);
    if (!ev) return;
    auto pass = m_policy.FindPassWithDepthState(ev->GetStateName());
    if (!pass) return;
    std::string pass_name = pass.value().get().m_program.m_programName;
    if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
    m_builtPassStates[pass_name].m_depth = ev->GetState();
    TryBuildEffectPass(pass_name);
}

void EffectCompiler::OnRasterizerStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceRasterizerStateCreated, IEvent>(e);
    if (!ev) return;
    auto pass = m_policy.FindPassWithRasterizerState(ev->GetStateName());
    if (!pass) return;
    std::string pass_name = pass.value().get().m_program.m_programName;
    if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
    m_builtPassStates[pass_name].m_rasterizer = ev->GetState();
    TryBuildEffectPass(pass_name);
}

void EffectCompiler::TryBuildEffectPass(const std::string& name)
{
    if (m_builtPrograms.find(name) == m_builtPrograms.end()) return;
    if (!m_builtPrograms[name]) return;
    auto it = m_builtPassStates.find(name);
    if (it == m_builtPassStates.end()) return;
    if (it->second.m_samplers)
    {
        for (unsigned int i = 0; i < it->second.m_samplers.value().size(); i++)
        {
            if (it->second.m_samplers.value()[i] == nullptr) return;
        }
    }
    if (!it->second.m_blend) return;
    if (!it->second.m_depth) return;
    if (!it->second.m_rasterizer) return;

    std::string tech_name;
    for (auto& tech : m_builtEffectTechniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_name == name)
            {
                pass.m_pass = EffectPass(m_builtPrograms[name], m_builtPassStates[name]);
                tech_name = tech.m_name;
            }
        }
    }
    TryBuildEffectTechniques(tech_name);
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
    std::vector<EffectTechnique> effect_techniques;
    for (auto& built_tech : m_builtEffectTechniques)
    {
        if (!built_tech.m_technique) return;
        effect_techniques.emplace_back(built_tech.m_technique.value());
    }
    auto effect_material = std::make_unique<EffectMaterial>(m_policy.m_name, effect_techniques);
    EventPublisher::Post(std::make_shared<EffectMaterialCompiled>(m_policy.m_name, std::move(effect_material)));
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

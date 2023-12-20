#include "EffectCompiler.h"
#include "ShaderEvents.h"
#include "ShaderCommands.h"
#include "EngineErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "GraphicKernel/GraphicCommands.h"
#include "GraphicKernel/GraphicEvents.h"
#include "EffectMaterialManager.h"
#include "EffectMaterialPolicy.h"
#include <cassert>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;

EffectCompiler::EffectCompiler() : m_ruidDeserializing()
{
    //m_hostManager = host;
    m_hasMaterialProduced = false;

    m_onCompilingProfileDeserialized = std::make_shared<EventSubscriber>([=](auto e) { this->onCompilingProfileDeserialized(e); });
    EventPublisher::subscribe(typeid(CompilingProfileDeserialized), m_onCompilingProfileDeserialized);
    m_onDeserializeCompilingProfileFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onDeserializeCompilingProfileFailed(e); });
    EventPublisher::subscribe(typeid(DeserializeCompilingProfileFailed), m_onDeserializeCompilingProfileFailed);

    m_onShaderProgramBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onShaderProgramBuilt(e); });
    EventPublisher::subscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onBuildProgramFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onBuildProgramFailed(e); });
    EventPublisher::subscribe(typeid(BuildShaderProgramFailed), m_onBuildProgramFailed);

    m_onSamplerStateCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onSamplerStateCreated(e); });
    EventPublisher::subscribe(typeid(DeviceSamplerStateCreated), m_onSamplerStateCreated);
    m_onBlendStateCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onBlendStateCreated(e); });
    EventPublisher::subscribe(typeid(DeviceAlphaBlendStateCreated), m_onBlendStateCreated);
    m_onDepthStateCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onDepthStateCreated(e); });
    EventPublisher::subscribe(typeid(DeviceDepthStencilStateCreated), m_onDepthStateCreated);
    m_onRasterizerStateCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onRasterizerStateCreated(e); });
    EventPublisher::subscribe(typeid(DeviceRasterizerStateCreated), m_onRasterizerStateCreated);
}

EffectCompiler::~EffectCompiler()
{
    EventPublisher::unsubscribe(typeid(CompilingProfileDeserialized), m_onCompilingProfileDeserialized);
    m_onCompilingProfileDeserialized = nullptr;
    EventPublisher::unsubscribe(typeid(DeserializeCompilingProfileFailed), m_onDeserializeCompilingProfileFailed);
    m_onDeserializeCompilingProfileFailed = nullptr;

    EventPublisher::unsubscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onShaderProgramBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(BuildShaderProgramFailed), m_onBuildProgramFailed);
    m_onBuildProgramFailed = nullptr;

    EventPublisher::unsubscribe(typeid(DeviceSamplerStateCreated), m_onSamplerStateCreated);
    m_onSamplerStateCreated = nullptr;
    EventPublisher::unsubscribe(typeid(DeviceAlphaBlendStateCreated), m_onBlendStateCreated);
    m_onBlendStateCreated = nullptr;
    EventPublisher::unsubscribe(typeid(DeviceDepthStencilStateCreated), m_onDepthStateCreated);
    m_onDepthStateCreated = nullptr;
    EventPublisher::unsubscribe(typeid(DeviceRasterizerStateCreated), m_onRasterizerStateCreated);
    m_onRasterizerStateCreated = nullptr;
}

EffectCompiler::CompilingProceed EffectCompiler::compileEffectMaterial(const EffectMaterialPolicy& policy)
{
    assert(m_hostManager);
    m_policy = policy;
    if (m_hostManager->hasEffectMaterial(policy.name()))
    {
        Frameworks::EventPublisher::post(std::make_shared<EffectMaterialCompiled>(
            policy.name(), m_hostManager->duplicateEffectMaterial(policy.name()), true));
        return CompilingProceed::False;
    }
    else if (auto p = policy.profile())
    {
        compileEffect(p.value());
    }
    else if (policy.deserializer())
    {
        m_ruidDeserializing = Ruid::generate();
        policy.deserializer()->invokeDeserialize(m_ruidDeserializing, policy.parameter());
    }
    else
    {
        EventPublisher::post(std::make_shared<CompileEffectMaterialFailed>(policy.name(), ErrorCode::policyIncomplete));
        return CompilingProceed::False;
    }
    return CompilingProceed::True;
}

void EffectCompiler::compileEffect(const EffectCompilingProfile& profile)
{
    m_profile = profile;
    m_builtPrograms.clear();
    m_builtPassStates.clear();
    m_builtEffectTechniques.clear();

    if (m_profile.m_techniques.empty())
    {
        EventPublisher::post(std::make_shared<CompileEffectMaterialFailed>(m_profile.m_name, ErrorCode::compilingEmptyEffectTech));
        return;
    }
    m_builtEffectTechniques.reserve(m_profile.m_techniques.size());
    for (auto& tech : m_profile.m_techniques)
    {
        BuiltEffectTechniqueMeta built_effect_technique{ tech.m_name, {}, std::nullopt };
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            BuiltEffectPassMeta built_effect_pass{ pass.m_name, std::nullopt };
            built_effect_technique.m_passes.emplace_back(built_effect_pass);
            if (m_builtPrograms.find(pass.m_program.m_programName) == m_builtPrograms.end())
            {
                m_builtPrograms.emplace(pass.m_program.m_programName, nullptr);
            }
            EffectPassStates built_pass_states;
            CommandBus::post(std::make_shared<BuildShaderProgram>(pass.m_program));
            if (!pass.m_samplers.empty())
            {
                built_pass_states.m_samplers = std::vector<Graphics::IDeviceSamplerStatePtr>{};
                built_pass_states.m_samplerNames = std::vector<std::string>{};
            }
            for (auto& samp : pass.m_samplers)
            {
                CommandBus::post(std::make_shared<CreateSamplerState>(samp.m_name, samp.m_data));
                if (built_pass_states.m_samplers) built_pass_states.m_samplers->emplace_back(nullptr);
                if (built_pass_states.m_samplerNames) built_pass_states.m_samplerNames->emplace_back(samp.m_variableName);
            }
            CommandBus::post(std::make_shared<CreateDepthStencilState>(pass.m_depth.m_name, pass.m_depth.m_data));
            CommandBus::post(std::make_shared<CreateBlendState>(pass.m_blend.m_name, pass.m_blend.m_data));
            CommandBus::post(std::make_shared<CreateRasterizerState>(pass.m_rasterizer.m_name, pass.m_rasterizer.m_data));
            m_builtPassStates.emplace(pass.m_name, built_pass_states);
        }
        m_builtEffectTechniques.emplace_back(built_effect_technique);
    }
    m_hasMaterialProduced = false;
}

void EffectCompiler::onCompilingProfileDeserialized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<CompilingProfileDeserialized, IEvent>(e);
    if (!ev) return;
    if (ev->ruid() != m_ruidDeserializing) return;
    compileEffect(ev->profile());
}

void EffectCompiler::onDeserializeCompilingProfileFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeserializeCompilingProfileFailed, IEvent>(e);
    if (!ev) return;
    EventPublisher::post(std::make_shared<CompileEffectMaterialFailed>(m_policy.name(), ev->error()));
}

void EffectCompiler::onShaderProgramBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_built = std::dynamic_pointer_cast<ShaderProgramBuilt, IEvent>(e);
    if (!ev_built) return;
    if (m_builtPrograms.find(ev_built->GetShaderName()) == m_builtPrograms.end()) return;
    m_builtPrograms[ev_built->GetShaderName()] = ev_built->GetProgram();
    tryBuildEffectPass(ev_built->GetShaderName());
}

void EffectCompiler::onBuildProgramFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev_fail = std::dynamic_pointer_cast<BuildShaderProgramFailed, IEvent>(e);
    if (!ev_fail) return;
    EventPublisher::post(std::make_shared<CompileEffectMaterialFailed>(m_profile.m_name, ev_fail->GetErrorCode()));
}

void EffectCompiler::onSamplerStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceSamplerStateCreated, IEvent>(e);
    if (!ev) return;
    const auto pass_profiles = m_profile.findPassesWithSamplerState(ev->GetStateName());
    if (pass_profiles.empty()) return;

    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        auto index = m_profile.findSamplerIndexInPass(pass_name, ev->GetStateName());
        if (!index) return;
        auto it = m_builtPassStates.find(pass_name);
        if (it == m_builtPassStates.end()) return;
        if (auto& s = it->second.m_samplers) s.value()[*index] = ev->GetState();
        tryBuildEffectPass(pass_name);
    }
}

void EffectCompiler::onBlendStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceAlphaBlendStateCreated, IEvent>(e);
    if (!ev) return;
    const auto pass_profiles = m_profile.findPassesWithBlendState(ev->GetStateName());
    if (pass_profiles.empty()) return;

    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
        m_builtPassStates[pass_name].m_blend = ev->GetState();
        tryBuildEffectPass(pass_name);
    }
}

void EffectCompiler::onDepthStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceDepthStencilStateCreated, IEvent>(e);
    if (!ev) return;
    const auto pass_profiles = m_profile.findPassesWithDepthState(ev->GetStateName());
    if (pass_profiles.empty()) return;

    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
        m_builtPassStates[pass_name].m_depth = ev->GetState();
        tryBuildEffectPass(pass_name);
    }
}

void EffectCompiler::onRasterizerStateCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeviceRasterizerStateCreated, IEvent>(e);
    if (!ev) return;
    const auto pass_profiles = m_profile.findPassesWithRasterizerState(ev->GetStateName());
    if (pass_profiles.empty()) return;

    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        if (m_builtPassStates.find(pass_name) == m_builtPassStates.end()) return;
        m_builtPassStates[pass_name].m_rasterizer = ev->GetState();
        tryBuildEffectPass(pass_name);
    }
}

void EffectCompiler::tryBuildEffectPass(const std::string& program_name)
{
    if (m_builtPrograms.find(program_name) == m_builtPrograms.end()) return;
    if (!m_builtPrograms[program_name]) return;
    const auto pass_profiles = m_profile.findPassesWithProgram(program_name);
    if (pass_profiles.empty()) return;
    for (auto& pass_profile : pass_profiles)
    {
        std::string pass_name = pass_profile.get().m_name;
        auto it = m_builtPassStates.find(pass_name);
        if (it == m_builtPassStates.end()) continue;
        if (auto samp = it->second.m_samplers)
        {
            for (unsigned int i = 0; i < samp.value().size(); i++)
            {
                if (samp.value()[i] == nullptr) continue;
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
        tryBuildEffectTechniques(tech_name);
    }
}

void EffectCompiler::tryBuildEffectTechniques(const std::string& name)
{
    for (auto& built_tech : m_builtEffectTechniques)
    {
        if (built_tech.m_name != name) continue;
        if (!built_tech.hasAllPassBuilt()) return;

        for (const auto& built_pass : built_tech.m_passes)
        {
            if (!built_pass.m_pass) return; // pass not yet built
        }
        built_tech.m_technique = EffectTechnique(built_tech.m_name, built_tech.retrieveEffectPasses());
    }
    tryBuildEffectMaterial();
}

void EffectCompiler::tryBuildEffectMaterial()
{
    if (m_hasMaterialProduced) return;
    std::vector<EffectTechnique> effect_techniques;
    for (auto& built_tech : m_builtEffectTechniques)
    {
        if (!built_tech.m_technique) return;
        effect_techniques.emplace_back(built_tech.m_technique.value());
    }
    auto effect_material = std::make_shared<EffectMaterial>(m_profile.m_name, effect_techniques);
    if (!m_policy.parameter().empty())
    {
        effect_material->factoryDesc().ClaimFromResource(effect_material->id().name(), m_policy.parameter());
    }
    EventPublisher::post(std::make_shared<EffectMaterialCompiled>(m_profile.m_name, effect_material, false));
    m_hasMaterialProduced = true;
}

//---------------------------------------------------------------------------------------------
bool EffectCompiler::BuiltEffectTechniqueMeta::hasAllPassBuilt()
{
    if (m_passes.empty()) return false;
    for (auto& built_pass : m_passes)
    {
        if (!built_pass.m_pass) return false;
    }
    return true;
}

std::vector<EffectPass> EffectCompiler::BuiltEffectTechniqueMeta::retrieveEffectPasses()
{
    if (m_passes.empty()) return {};
    std::vector<EffectPass> effect_passes;
    effect_passes.reserve(m_passes.size());
    for (auto& built_pass : m_passes)
    {
        if (auto p = built_pass.m_pass) effect_passes.emplace_back(p.value());
    }
    return effect_passes;
}

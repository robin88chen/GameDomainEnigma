#include "EffectCompiler.h"
#include "ShaderEvents.h"
#include "ShaderCommands.h"
#include "EffectEvents.h"
#include "EngineErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

EffectCompiler::EffectCompiler()
{
    m_onShaderProgramBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnShaderProgramBuilt(e); });
    EventPublisher::Subscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onProgramBuildFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnProgramBuildFailed(e); });
    EventPublisher::Subscribe(typeid(ShaderProgramBuildFailed), m_onProgramBuildFailed);
}

EffectCompiler::~EffectCompiler()
{
    EventPublisher::Unsubscribe(typeid(ShaderProgramBuilt), m_onShaderProgramBuilt);
    m_onShaderProgramBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(ShaderProgramBuildFailed), m_onProgramBuildFailed);
    m_onProgramBuildFailed = nullptr;
}

void EffectCompiler::CompileEffect(const EffectCompilingPolicy& policy)
{
    m_policy = policy;
    if (m_policy.m_techniques.empty())
    {
        EventPublisher::Post(std::make_shared<EffectMaterialCompileFailed>(m_policy.m_name, ErrorCode::compilingEmptyEffectTech));
        return;
    }
    m_builtEffectTechniques.reserve(m_policy.m_techniques.size());
    for (auto& tech : m_policy.m_techniques)
    {
        BuiltEffectTechnique built_effect_technique{ m_policy.m_name };
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            BuiltEffectPass built_effect_pass{ pass.m_program.m_programName };
            built_effect_technique.m_passes.emplace_back(built_effect_pass);
            m_builtPrograms.emplace(pass.m_program.m_programName, nullptr);
            m_builtPassStates.emplace(pass.m_program.m_programName, EffectPassStates{});
            CommandBus::Post(std::make_shared<BuildShaderProgram>(pass.m_program));
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

    for (auto& tech : m_builtEffectTechniques)
    {
        if (tech.m_passes.empty()) continue;
        for (auto& pass : tech.m_passes)
        {
            if (pass.m_name == name)
            {
                pass.m_pass = EffectPass(m_builtPrograms[name], m_builtPassStates[name]);
            }
        }
    }
}

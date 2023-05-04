#include "EffectPass.h"
#include "MaterialVariableMap.h"
#include "GraphicKernel/IShaderProgram.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "GraphicKernel/IDeviceAlphaBlendState.h"
#include "GraphicKernel/IDeviceDepthStencilState.h"
#include "GraphicKernel/IDeviceRasterizerState.h"
#include <cassert>

using namespace Enigma::Graphics;
using namespace Enigma::Engine;

EffectPass::EffectPass(const std::string& name, const Graphics::IShaderProgramPtr& program, const EffectPassStates& states)
{
    m_name = name;
    m_shader = program;
    if (states.m_samplers) m_samplers = states.m_samplers.value();
    if (states.m_samplerNames) m_samplerNames = states.m_samplerNames.value();
    if (states.m_rasterizer) m_rasterizerState = states.m_rasterizer.value();
    if (states.m_depth) m_depthState = states.m_depth.value();
    if (states.m_blend) m_blendState = states.m_blend.value();
    BuildVariables();
}

EffectPass::EffectPass(const EffectPass& pass)
{
    m_name = pass.m_name;
    m_shader = pass.m_shader;
    m_samplers = pass.m_samplers;
    m_samplerNames = pass.m_samplerNames;
    m_rasterizerState = pass.m_rasterizerState;
    m_depthState = pass.m_depthState;
    m_blendState = pass.m_blendState;
    m_variables = pass.m_variables;
}

EffectPass::EffectPass(EffectPass&& pass) noexcept
{
    m_name = std::move(pass.m_name);
    m_shader = std::move(pass.m_shader);
    m_samplers = std::move(pass.m_samplers);
    m_samplerNames = std::move(pass.m_samplerNames);
    m_rasterizerState = std::move(pass.m_rasterizerState);
    m_depthState = std::move(pass.m_depthState);
    m_blendState = std::move(pass.m_blendState);
    m_variables = std::move(pass.m_variables);
}

EffectPass::~EffectPass()
{
    m_shader = nullptr;
    m_samplers.clear();
    m_samplerNames.clear();
    m_rasterizerState = nullptr;
    m_depthState = nullptr;
    m_blendState = nullptr;
    m_variables.clear();
}

EffectPass& EffectPass::operator=(const EffectPass& pass)
{
    if (this == &pass) return *this;
    m_name = pass.m_name;
    m_shader = pass.m_shader;
    m_samplers = pass.m_samplers;
    m_samplerNames = pass.m_samplerNames;
    m_rasterizerState = pass.m_rasterizerState;
    m_depthState = pass.m_depthState;
    m_blendState = pass.m_blendState;
    m_variables = pass.m_variables;

    return *this;
}

EffectPass& EffectPass::operator=(EffectPass&& pass) noexcept
{
    m_name = std::move(pass.m_name);
    m_shader = std::move(pass.m_shader);
    m_samplers = std::move(pass.m_samplers);
    m_samplerNames = std::move(pass.m_samplerNames);
    m_rasterizerState = std::move(pass.m_rasterizerState);
    m_depthState = std::move(pass.m_depthState);
    m_blendState = std::move(pass.m_blendState);
    m_variables = std::move(pass.m_variables);

    return *this;
}

void EffectPass::MappingAutoVariables()
{
    if (!MaterialVariableMap::Instance()) return;
    for (auto& var : m_variables)
    {
        MaterialVariableMap::SetAutoVariableCommitFunction(var);
    }
}

void EffectPass::CommitVariables()
{
    if (!m_samplers.empty())
    {
        for (unsigned int i = 0; i < m_samplers.size(); i++)
        {
            auto var = GetVariableByName(m_samplerNames[i]);
            if (var)
            {
                var.value().get().AssignValue(m_samplers[i]);
            }
        }
    }
    for (auto& var : m_variables)
    {
        var.Commit();
    }
}

void EffectPass::Apply()
{
    // 每個 api call 會自行處理 async
    if (m_shader)
    {
        IGraphicAPI::Instance()->Bind(m_shader);
        m_shader->ApplyVariables();
    }
    if (m_blendState)
    {
        m_blendState->Bind();
    }
    if (m_depthState)
    {
        m_depthState->Bind();
    }
    if (m_rasterizerState)
    {
        m_rasterizerState->Bind();
    }
}

stdext::optional_ref<EffectVariable> EffectPass::GetVariableByName(const std::string& name)
{
    if (m_variables.empty()) return std::nullopt;
    for (unsigned int i = 0; i < m_variables.size(); i++)
    {
        if (m_variables[i].GetName() == name) return m_variables[i];
    }

    return std::nullopt;
}

stdext::optional_ref<EffectVariable> EffectPass::GetVariableBySemantic(const std::string& semantic)
{
    if (m_variables.empty()) return std::nullopt;
    for (unsigned int i = 0; i < m_variables.size(); i++)
    {
        if (m_variables[i].GetSemantic() == semantic) return m_variables[i];
    }
    return std::nullopt;
}

stdext::optional_ref<const EffectVariable> EffectPass::GetVariableByName(const std::string& name) const
{
    if (m_variables.empty()) return std::nullopt;
    for (unsigned int i = 0; i < m_variables.size(); i++)
    {
        if (m_variables[i].GetName() == name) return m_variables[i];
    }
    return std::nullopt;
}

stdext::optional_ref<const EffectVariable> EffectPass::GetVariableBySemantic(const std::string& semantic) const
{
    if (m_variables.empty()) return std::nullopt;
    for (unsigned int i = 0; i < m_variables.size(); i++)
    {
        if (m_variables[i].GetSemantic() == semantic) return m_variables[i];
    }
    return std::nullopt;
}

void EffectPass::BuildVariables()
{
    assert(m_shader);
    if (m_shader->GetVariableCount() == 0) return;
    m_variables.reserve(m_shader->GetVariableCount());
    for (unsigned int i = 0; i < m_shader->GetVariableCount(); i++)
    {
        m_variables.emplace_back(m_shader->GetVariableByIndex(i));
    }
}

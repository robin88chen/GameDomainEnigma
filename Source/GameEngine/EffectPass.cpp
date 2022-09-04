#include "EffectPass.h"
#include "GraphicKernel/IShaderProgram.h"
#include "GraphicKernel/IGraphicAPI.h"
#include <cassert>

#include "GraphicKernel/IDeviceAlphaBlendState.h"
#include "GraphicKernel/IDeviceDepthStencilState.h"
#include "GraphicKernel/IDeviceRasterizerState.h"

using namespace Enigma::Graphics;
using namespace Enigma::Engine;

EffectPass::EffectPass(const Graphics::IShaderProgramPtr& program, const EffectPassStates& states)
{
    m_shader = program;
    if (states.m_samplers) m_samplers = states.m_samplers.value();
    if (states.m_sampler_names) m_sampler_names = states.m_sampler_names.value();
    if (states.m_rasterizer) m_rasterizer_state = states.m_rasterizer.value();
    if (states.m_depth) m_depth_state = states.m_depth.value();
    if (states.m_blend) m_blend_state = states.m_blend.value();
    BuildVariables();
}

EffectPass::EffectPass(const EffectPass& pass)
{
    m_shader = pass.m_shader;
    m_samplers = pass.m_samplers;
    m_sampler_names = pass.m_sampler_names;
    m_rasterizer_state = pass.m_rasterizer_state;
    m_depth_state = pass.m_depth_state;
    m_blend_state = pass.m_blend_state;
    m_variables = pass.m_variables;
}

EffectPass::EffectPass(EffectPass&& pass)
{
    m_shader = std::move(pass.m_shader);
    m_samplers = std::move(pass.m_samplers);
    m_sampler_names = std::move(pass.m_sampler_names);
    m_rasterizer_state = std::move(pass.m_rasterizer_state);
    m_depth_state = std::move(pass.m_depth_state);
    m_blend_state = std::move(pass.m_blend_state);
    m_variables = std::move(pass.m_variables);
}

EffectPass::~EffectPass()
{
    m_shader = nullptr;
    m_samplers.clear();
    m_sampler_names.clear();
    m_rasterizer_state = nullptr;
    m_depth_state = nullptr;
    m_blend_state = nullptr;
    m_variables.clear();
}

EffectPass& EffectPass::operator=(const EffectPass& pass)
{
    m_shader = pass.m_shader;
    m_samplers = pass.m_samplers;
    m_sampler_names = pass.m_sampler_names;
    m_rasterizer_state = pass.m_rasterizer_state;
    m_depth_state = pass.m_depth_state;
    m_blend_state = pass.m_blend_state;
    m_variables = pass.m_variables;

    return *this;
}

EffectPass& EffectPass::operator=(EffectPass&& pass) noexcept
{
    m_shader = std::move(pass.m_shader);
    m_samplers = std::move(pass.m_samplers);
    m_sampler_names = std::move(pass.m_sampler_names);
    m_rasterizer_state = std::move(pass.m_rasterizer_state);
    m_depth_state = std::move(pass.m_depth_state);
    m_blend_state = std::move(pass.m_blend_state);
    m_variables = std::move(pass.m_variables);

    return *this;
}

void EffectPass::CommitVariables()
{
    if (!m_samplers.empty())
    {
        for (unsigned int i = 0; i < m_samplers.size(); i++)
        {
            auto var = GetVariableByName(m_sampler_names[i]);
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
    if (m_blend_state)
    {
        m_blend_state->Bind();
    }
    if (m_depth_state)
    {
        m_depth_state->Bind();
    }
    if (m_rasterizer_state)
    {
        m_rasterizer_state->Bind();
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

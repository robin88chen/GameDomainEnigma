#include "EffectVariable.h"

using namespace Enigma::Graphics;
using namespace Enigma::Engine;

EffectVariable::EffectVariable(const Graphics::IShaderVariablePtr& shader_variable)
{
    m_shader_variable = shader_variable;
    m_name = m_shader_variable->GetVariableName();
    m_semantic = m_shader_variable->GetVariableSemantic();
    m_value_count = 0;
    m_assign = nullptr;
}

EffectVariable::EffectVariable(const EffectVariable& var)
{
    m_shader_variable = var.m_shader_variable;
    m_name = var.m_name;
    m_semantic = var.m_semantic;
    m_value = var.m_value;
    m_value_count = var.m_value_count;
    m_assign = var.m_assign;
}

EffectVariable::EffectVariable(EffectVariable&& var) noexcept
{
    m_shader_variable = std::move(var.m_shader_variable);
    m_name = std::move(var.m_name);
    m_semantic = std::move(var.m_semantic);
    m_value = std::move(var.m_value);
    m_value_count = var.m_value_count;
    m_assign = std::move(var.m_assign);
}

EffectVariable::~EffectVariable()
{
    m_shader_variable = nullptr;
    m_assign = nullptr;
}

EffectVariable& EffectVariable::operator=(const EffectVariable& var)
{
    m_shader_variable = var.m_shader_variable;
    m_name = var.m_name;
    m_semantic = var.m_semantic;
    m_value = var.m_value;
    m_value_count = var.m_value_count;
    m_assign = var.m_assign;
    return *this;
}

EffectVariable& EffectVariable::operator=(EffectVariable&& var) noexcept
{
    m_shader_variable = std::move(var.m_shader_variable);
    m_name = std::move(var.m_name);
    m_semantic = std::move(var.m_semantic);
    m_value = std::move(var.m_value);
    m_value_count = var.m_value_count;
    m_assign = std::move(var.m_assign);
    return *this;
}

void EffectVariable::AssignValue(std::any value)
{
    m_value = std::move(value);
    m_value_count = 1;
}

void EffectVariable::AssignValues(std::any value_array, unsigned value_count)
{
    m_value = std::move(value_array);
    m_value_count = value_count;
}

void EffectVariable::Commit()
{
    if (m_assign) m_assign(*this);
    if (!m_shader_variable) return;
    if (m_value_count == 0) return;
    if (m_value_count == 1)
    {
        m_shader_variable->SetValue(m_value);
    }
    else
    {
        m_shader_variable->SetValues(m_value, m_value_count);
    }
}

void EffectVariable::SetValueAssignFunction(VariableValueAssignFunc fn)
{
    m_assign = std::move(fn);
}


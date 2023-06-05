#include "EffectVariable.h"

using namespace Enigma::Graphics;
using namespace Enigma::Engine;

EffectVariable::EffectVariable(const Graphics::IShaderVariablePtr& shader_variable)
{
    m_shaderVariable = shader_variable;
    m_name = m_shaderVariable->GetVariableName();
    m_semantic = m_shaderVariable->GetVariableSemantic();
    m_valueCount = std::nullopt;
    m_assign = nullptr;
}

EffectVariable::EffectVariable(const EffectVariable& var)
{
    m_shaderVariable = var.m_shaderVariable;
    m_name = var.m_name;
    m_semantic = var.m_semantic;
    m_value = var.m_value;
    m_valueCount = var.m_valueCount;
    m_assign = var.m_assign;
}

EffectVariable::EffectVariable(EffectVariable&& var) noexcept
{
    m_shaderVariable = std::move(var.m_shaderVariable);
    m_name = std::move(var.m_name);
    m_semantic = std::move(var.m_semantic);
    m_value = std::move(var.m_value);
    m_valueCount = var.m_valueCount;
    m_assign = std::move(var.m_assign);
}

EffectVariable::~EffectVariable()
{
    m_shaderVariable = nullptr;
    m_assign = nullptr;
}

EffectVariable& EffectVariable::operator=(const EffectVariable& var)
{
    if (this == &var) return *this;
    m_shaderVariable = var.m_shaderVariable;
    m_name = var.m_name;
    m_semantic = var.m_semantic;
    m_value = var.m_value;
    m_valueCount = var.m_valueCount;
    m_assign = var.m_assign;
    return *this;
}

EffectVariable& EffectVariable::operator=(EffectVariable&& var) noexcept
{
    m_shaderVariable = std::move(var.m_shaderVariable);
    m_name = std::move(var.m_name);
    m_semantic = std::move(var.m_semantic);
    m_value = std::move(var.m_value);
    m_valueCount = var.m_valueCount;
    m_assign = std::move(var.m_assign);
    return *this;
}

bool EffectVariable::operator==(const EffectVariable& var)
{
    if (m_shaderVariable != var.m_shaderVariable) return false;
    if (m_name != var.m_name) return false;
    if (m_semantic != var.m_semantic) return false;
    return true;
}

void EffectVariable::AssignValue(std::any value)
{
    m_value = std::move(value);
}

void EffectVariable::AssignValues(std::any value_array, unsigned value_count)
{
    m_value = std::move(value_array);
    m_valueCount = value_count;
}

void EffectVariable::Commit()
{
    if (m_assign) m_assign(*this);
    if (!m_shaderVariable) return;
    if (!m_value.has_value()) return;
    if (!m_valueCount)
    {
        m_shaderVariable->SetValue(m_value);
    }
    else
    {
        m_shaderVariable->SetValues(m_value, m_valueCount.value());
    }
}

void EffectVariable::SetValueAssignFunction(VariableValueAssignFunc fn)
{
    m_assign = std::move(fn);
}


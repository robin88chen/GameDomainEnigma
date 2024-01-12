/*********************************************************************
 * \file   EffectVariable.h
 * \brief  Effect Variable value object
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef EFFECT_VARIABLE_H
#define EFFECT_VARIABLE_H

#include "GraphicKernel/IShaderVariable.h"

namespace Enigma::Engine
{
    class EffectVariable
    {
    public:
        using VariableValueAssignFunc = std::function<void(EffectVariable&)>;
    public:
        EffectVariable(const Graphics::IShaderVariablePtr& shader_variable);
        EffectVariable(const EffectVariable& var);
        EffectVariable(EffectVariable&& var) noexcept;
        ~EffectVariable();

        EffectVariable& operator=(const EffectVariable& var);
        EffectVariable& operator=(EffectVariable&& var) noexcept;

        bool operator==(const EffectVariable& var);

        const std::string& name() { return m_name; };
        const std::string& semantic() { return m_semantic; };
        const std::string& name() const { return m_name; };
        const std::string& semantic() const { return m_semantic; };

        void assignValue(std::any value);
        void assignValues(std::any value_array, unsigned int value_count);

        /** commit : 先呼叫從 Variable Map 或是其他外部掛入的 assign 函式, then set shader variable value(s) */
        void commit();
        /** assign 函式 : assign value(s) */
        void setValueAssignFunction(VariableValueAssignFunc fn);

    private:
        Graphics::IShaderVariablePtr m_shaderVariable;
        std::string m_name;
        std::string m_semantic;
        std::any m_value;
        std::optional<unsigned> m_valueCount;
        VariableValueAssignFunc m_assign;
    };
}

#endif // EFFECT_VARIABLE_H

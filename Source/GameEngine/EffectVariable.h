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
        using VariableCommitFunc = std::function<void(const EffectVariable&)>;
    public:
        EffectVariable(const Graphics::IShaderVariablePtr& shader_variable);
        EffectVariable(const EffectVariable& var);
        EffectVariable(EffectVariable&& var) noexcept;
        ~EffectVariable();

        EffectVariable& operator=(const EffectVariable& var);
        EffectVariable& operator=(EffectVariable&& var) noexcept;

        const std::string& GetName() { return m_name; };
        const std::string& GetSemantic() { return m_semantic; };

        void AssignValue(std::any value);
        void AssignValues(std::any value_array, unsigned int value_count);

        /** commit : set shader variable value(s), or 呼叫從 Variable Map 或是其他外部掛入的 commit 函式 */
        void Commit();
        /** commit 函式需要直接呼叫 shader variable set value */
        void SetCommitFunction(VariableCommitFunc fn);

    private:
        Graphics::IShaderVariablePtr m_shader_variable;
        std::string m_name;
        std::string m_semantic;
        std::any m_value;
        unsigned int m_value_count;
        VariableCommitFunc m_commit;
    };
}

#endif // EFFECT_VARIABLE_H

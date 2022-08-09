/*********************************************************************
 * \file   ShaderCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef SHADER_COMMANDS_H
#define SHADER_COMMANDS_H

#include "Frameworks/Command.h"
#include "ShaderBuildingPolicies.h"

namespace Enigma::Engine
{
    class BuildShaderProgram : public Frameworks::ICommand
    {
    public:
        BuildShaderProgram(const ShaderProgramPolicy& policy) : m_policy(policy) {}
        const ShaderProgramPolicy& GetPolicy() { return m_policy; }

    private:
        ShaderProgramPolicy m_policy;
    };
}


#endif // SHADER_COMMANDS_H

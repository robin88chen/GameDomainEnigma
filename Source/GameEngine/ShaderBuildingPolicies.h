/*********************************************************************
 * \file   ShaderBuildingPolicies.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   August 2022
 *********************************************************************/
#ifndef SHADER_BUILDING_POLICIES_H
#define SHADER_BUILDING_POLICIES_H

#include <string>

namespace Enigma::Engine
{
    struct ShaderCodeProfile
    {
        std::string m_code;
        std::string m_profile;
        std::string m_entry;
    };
    struct ShaderProgramPolicy
    {
        std::string m_programName;
        std::string m_vtxShaderName;
        std::string m_vtxFormatCode;
        ShaderCodeProfile m_vtxShaderCode;
        std::string m_vtxLayoutName;
        std::string m_pixShaderName;
        ShaderCodeProfile m_pixShaderCode;
    };
}

#endif // SHADER_BUILDING_POLICIES_H

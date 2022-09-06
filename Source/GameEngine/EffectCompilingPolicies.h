/*********************************************************************
 * \file   EffectCompilingPolicies.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef EFFECT_COMPILING_POLICIES_H
#define EFFECT_COMPILING_POLICIES_H

#include "ShaderBuildingPolicies.h"
#include "GraphicKernel/IDeviceSamplerState.h"
#include "GraphicKernel/IDeviceRasterizerState.h"
#include "GraphicKernel/IDeviceAlphaBlendState.h"
#include "GraphicKernel/IDeviceDepthStencilState.h"
#include "Frameworks/optional_ref.hpp"
#include <string>

namespace Enigma::Engine
{
    struct EffectSamplerProfile
    {
        std::string m_name;
        Graphics::IDeviceSamplerState::SamplerStateData m_data;
        std::string m_variableName;
    };
    struct EffectRasterizerProfile
    {
        std::string m_name;
        Graphics::IDeviceRasterizerState::RasterizerStateData m_data;
    };
    struct EffectAlphaBlendProfile
    {
        std::string m_name;
        Graphics::IDeviceAlphaBlendState::BlendStateData m_data;
    };
    struct EffectDepthStencilProfile
    {
        std::string m_name;
        Graphics::IDeviceDepthStencilState::DepthStencilData m_data;
    };
    struct EffectPassProfile
    {
        ShaderProgramPolicy m_program;
        std::vector<EffectSamplerProfile> m_samplers;
        EffectRasterizerProfile m_rasterizer;
        EffectAlphaBlendProfile m_blend;
        EffectDepthStencilProfile m_depth;
    };
    struct EffectTechniqueProfile
    {
        std::string m_name;
        std::vector<EffectPassProfile> m_passes;
    };
    struct EffectCompilingPolicy
    {
        std::string m_name;
        std::vector<EffectTechniqueProfile> m_techniques;
        stdext::optional_ref<EffectPassProfile> FindPassWithSamplerState(const std::string& name);
        stdext::optional_ref<EffectPassProfile> FindPassWithBlendState(const std::string& name);
        stdext::optional_ref<EffectPassProfile> FindPassWithDepthState(const std::string& name);
        stdext::optional_ref<EffectPassProfile> FindPassWithRasterizerState(const std::string& name);
        std::optional<unsigned int> FindSamplerIndexInPass(const std::string& name);
    };

}

#endif // EFFECT_COMPILING_POLICIES_H

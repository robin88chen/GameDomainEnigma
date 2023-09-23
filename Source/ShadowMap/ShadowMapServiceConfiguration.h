/*********************************************************************
 * \file   ShadowMapServiceConfiguration.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _SHADOW_MAP_SERVICE_CONFIGURATION_H
#define _SHADOW_MAP_SERVICE_CONFIGURATION_H
#include <string>

#include "MathLib/AlgebraBasicTypes.h"

namespace Enigma::ShadowMap
{
    class ShadowMapServiceConfiguration
    {
    public:
        ShadowMapServiceConfiguration();

        const std::string& ShadowMapTechniqueName() const;
        std::string& ShadowMapTechniqueName();

        const MathLib::Dimension<unsigned>& ShadowMapDimension() const;
        MathLib::Dimension<unsigned>& ShadowMapDimension();

        const std::string& ShadowMapSurfaceName() const;
        std::string& ShadowMapSurfaceName();
        const std::string& ShadowMapDepthName() const;
        std::string& ShadowMapDepthName();

        const std::string& SunLightCameraName() const;
        std::string& SunLightCameraName();

        const std::string& LightViewProjSemantic() const;
        std::string& LightViewProjSemantic();

        const std::string& ShadowMapSemantic() const;
        std::string& ShadowMapSemantic();

        const std::string& ShadowMapDimensionSemantic() const;
        std::string& ShadowMapDimensionSemantic();

        float ShadowMapDepthBias() const;
        float& ShadowMapDepthBias();
        float ShadowMapDensity() const;
        float& ShadowMapDensity();

    protected:
        std::string m_shadowMapTechniqueName;
        MathLib::Dimension<unsigned> m_shadowMapDimension;
        std::string m_shadowMapSurfaceName;
        std::string m_shadowMapDepthName;
        std::string m_sunLightCameraName;
        std::string m_lightViewProjSemantic;
        std::string m_shadowMapSemantic;
        std::string m_shadowMapDimensionSemantic;
        float m_shadowMapDepthBias;
        float m_shadowMapDensity;
    };
}

#endif // _SHADOW_MAP_SERVICE_CONFIGURATION_H

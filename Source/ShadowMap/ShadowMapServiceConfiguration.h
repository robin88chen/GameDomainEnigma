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

        const MathLib::Dimension& ShadowMapDimension() const;
        MathLib::Dimension& ShadowMapDimension();

        const std::string& ShadowMapSurfaceName() const;
        std::string& ShadowMapSurfaceName();
        const std::string& ShadowMapDepthName() const;
        std::string& ShadowMapDepthName();

        const std::string& SunLightCameraName() const;
        std::string& SunLightCameraName();

    protected:
        std::string m_shadowMapTechniqueName;
        MathLib::Dimension m_shadowMapDimension;
        std::string m_shadowMapSurfaceName;
        std::string m_shadowMapDepthName;
        std::string m_sunLightCameraName;
    };
}

#endif // _SHADOW_MAP_SERVICE_CONFIGURATION_H

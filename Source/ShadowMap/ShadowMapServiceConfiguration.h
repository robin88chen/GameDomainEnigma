/*********************************************************************
 * \file   ShadowMapServiceConfiguration.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _SHADOW_MAP_SERVICE_CONFIGURATION_H
#define _SHADOW_MAP_SERVICE_CONFIGURATION_H

#include "MathLib/AlgebraBasicTypes.h"
#include "SceneGraph/SpatialId.h"
#include "GameEngine/TextureId.h"
#include <string>

namespace Enigma::ShadowMap
{
    class ShadowMapServiceConfiguration
    {
    public:
        ShadowMapServiceConfiguration();

        const std::string& shadowMapTechniqueName() const;
        std::string& shadowMapTechniqueName();

        const MathLib::Dimension<unsigned>& shadowMapDimension() const;
        MathLib::Dimension<unsigned>& shadowMapDimension();

        const std::string& shadowMapSurfaceName() const;
        std::string& shadowMapSurfaceName();
        const std::string& shadowMapDepthName() const;
        std::string& shadowMapDepthName();

        const SceneGraph::SpatialId& sunLightCameraId() const;
        SceneGraph::SpatialId& sunLightCameraId();

        const std::string& lightViewProjSemantic() const;
        std::string& lightViewProjSemantic();

        const std::string& shadowMapSemantic() const;
        std::string& shadowMapSemantic();

        const std::string& shadowMapDimensionSemantic() const;
        std::string& shadowMapDimensionSemantic();

        const Engine::TextureId& shadowMapTextureId() const;
        Engine::TextureId& shadowMapTextureId();

        float shadowMapDepthBias() const;
        float& shadowMapDepthBias();
        float shadowMapDensity() const;
        float& shadowMapDensity();

    protected:
        std::string m_shadowMapTechniqueName;
        MathLib::Dimension<unsigned> m_shadowMapDimension;
        std::string m_shadowMapSurfaceName;
        std::string m_shadowMapDepthName;
        SceneGraph::SpatialId m_sunLightCameraId;
        std::string m_lightViewProjSemantic;
        std::string m_shadowMapSemantic;
        std::string m_shadowMapDimensionSemantic;
        Engine::TextureId m_shadowMapTextureId;
        float m_shadowMapDepthBias;
        float m_shadowMapDensity;
    };
}

#endif // _SHADOW_MAP_SERVICE_CONFIGURATION_H

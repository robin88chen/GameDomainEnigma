/*********************************************************************
 * \file   DeferredRendererServiceConfiguration.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef DEFERRED_RENDERER_SERVICE_CONFIGURATION_H
#define DEFERRED_RENDERER_SERVICE_CONFIGURATION_H
#include <string>
#include <vector>
#include "GraphicKernel/RenderTextureUsage.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "SceneRendererServiceConfiguration.h"
#include "SceneGraph/Spatial.h"
#include "GameEngine/EffectMaterialId.h"

namespace Enigma::GameCommon
{
    class DeferredRendererServiceConfiguration : public SceneRendererServiceConfiguration
    {
    public:
        DeferredRendererServiceConfiguration();

        const Engine::EffectMaterialId& ambientEffect() const;
        Engine::EffectMaterialId& ambientEffect();
        const Engine::EffectMaterialId& sunLightEffect() const;
        Engine::EffectMaterialId& sunLightEffect();
        const Engine::EffectMaterialId& lightVolumeEffect() const;
        Engine::EffectMaterialId& lightVolumeEffect();

        const std::string& deferredRendererTechniqueName() const;
        std::string& deferredRendererTechniqueName();
        const std::string& visualTechniqueNameForCameraDefault() const;
        std::string& visualTechniqueNameForCameraDefault();
        const std::string& visualTechniqueNameForCameraInside() const;
        std::string& visualTechniqueNameForCameraInside();

        const std::string& gbufferTargetName() const;
        std::string& gbufferTargetName();
        const std::string& gbufferSurfaceName() const;
        std::string& gbufferSurfaceName();
        const std::string& gbufferDepthName() const;
        std::string& gbufferDepthName();

        const std::vector<Graphics::RenderTextureUsage>& gbufferUsages() const;
        std::vector<Graphics::RenderTextureUsage>& gbufferUsages();
        const std::vector<Graphics::GraphicFormat>& gbufferFormats() const;
        std::vector<Graphics::GraphicFormat>& gbufferFormats();

        const std::string& gbufferNormalSemantic() const;
        std::string& gbufferNormalSemantic();
        const std::string& gbufferDiffuseSemantic() const;
        std::string& gbufferDiffuseSemantic();
        const std::string& gbufferSpecularSemantic() const;
        std::string& gbufferSpecularSemantic();
        const std::string& gbufferDepthSemantic() const;
        std::string& gbufferDepthSemantic();

        const SceneGraph::Spatial::SpatialFlags& sunLightSpatialFlags() const;
        SceneGraph::Spatial::SpatialFlags& sunLightSpatialFlags();

    protected:
        Engine::EffectMaterialId m_ambientEffect;
        Engine::EffectMaterialId m_sunLightEffect;
        Engine::EffectMaterialId m_lightVolumeEffect;

        std::string m_deferredRendererTechniqueName;
        std::string m_visualTechniqueNameForCameraDefault;
        std::string m_visualTechniqueNameForCameraInside;
        std::string m_gbufferTargetName;
        std::string m_gbufferSurfaceName;
        std::string m_gbufferDepthName;

        std::vector<Graphics::RenderTextureUsage> m_gbufferUsages;
        std::vector<Graphics::GraphicFormat> m_gbufferFormats;

        std::string m_gbufferNormalSemantic;
        std::string m_gbufferDiffuseSemantic;
        std::string m_gbufferSpecularSemantic;
        std::string m_gbufferDepthSemantic;

        SceneGraph::Spatial::SpatialFlags m_sunLightSpatialFlags;
    };
}

#endif // DEFERRED_RENDERER_SERVICE_CONFIGURATION_H

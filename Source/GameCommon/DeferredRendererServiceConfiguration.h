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

namespace Enigma::GameCommon
{
    class DeferredRendererServiceConfiguration : public SceneRendererServiceConfiguration
    {
    public:
        DeferredRendererServiceConfiguration();

        const std::string& AmbientPassFxFileName() const;
        std::string& AmbientPassFxFileName();
        const std::string& AmbientEffectName() const;
        std::string& AmbientEffectName();
        const std::string& SunLightPassFxFileName() const;
        std::string& SunLightPassFxFileName();
        const std::string& SunLightEffectName() const;
        std::string& SunLightEffectName();
        const std::string& LightVolumePassFxFileName() const;
        std::string& LightVolumePassFxFileName();
        const std::string& LightVolumeEffectName() const;
        std::string& LightVolumeEffectName();

        const std::string& DeferredRendererTechniqueName() const;
        std::string& DeferredRendererTechniqueName();
        const std::string& VisualTechniqueNameForCameraDefault() const;
        std::string& VisualTechniqueNameForCameraDefault();
        const std::string& VisualTechniqueNameForCameraInside() const;
        std::string& VisualTechniqueNameForCameraInside();

        const std::string& GbufferTargetName() const;
        std::string& GbufferTargetName();
        const std::string& GbufferSurfaceName() const;
        std::string& GbufferSurfaceName();
        const std::string& GbufferDepthName() const;
        std::string& GbufferDepthName();

        const std::vector<Graphics::RenderTextureUsage>& GbufferUsages() const;
        std::vector<Graphics::RenderTextureUsage>& GbufferUsages();
        const std::vector<Graphics::GraphicFormat>& GbufferFormats() const;
        std::vector<Graphics::GraphicFormat>& GbufferFormats();

        const std::string& GbufferNormalSemantic() const;
        std::string& GbufferNormalSemantic();
        const std::string& GbufferDiffuseSemantic() const;
        std::string& GbufferDiffuseSemantic();
        const std::string& GbufferSpecularSemantic() const;
        std::string& GbufferSpecularSemantic();
        const std::string& GbufferDepthSemantic() const;
        std::string& GbufferDepthSemantic();

        const SceneGraph::Spatial::SpatialFlags& SunLightSpatialFlags() const;
        SceneGraph::Spatial::SpatialFlags& SunLightSpatialFlags();

    protected:
        std::string m_ambientPassFxFileName;
        std::string m_ambientEffectName;
        std::string m_sunLightPassFxFileName;
        std::string m_sunLightEffectName;
        std::string m_lightVolumePassFxFileName;
        std::string m_lightVolumeEffectName;

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

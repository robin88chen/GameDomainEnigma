/*********************************************************************
 * \file   DeferredRenderingConfiguration.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef DEFERRED_RENDERING_CONFIGURATION_H
#define DEFERRED_RENDERING_CONFIGURATION_H

#include "GraphicKernel/RenderTextureUsage.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "SceneRenderingConfiguration.h"
#include "SceneGraph/Spatial.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/TextureId.h"
#include <string>
#include <vector>

namespace Enigma::Rendering
{
    class DeferredRenderingConfiguration : public SceneRenderingConfiguration
    {
    public:
        DeferredRenderingConfiguration();

        const Engine::EffectMaterialId& ambientEffect() const;
        void ambientEffect(const Engine::EffectMaterialId&);
        const Engine::EffectMaterialId& sunLightEffect() const;
        void sunLightEffect(const Engine::EffectMaterialId&);
        const Engine::EffectMaterialId& lightVolumeEffect() const;
        void lightVolumeEffect(const Engine::EffectMaterialId&);

        const std::string& deferredRendererTechniqueName() const;
        void deferredRendererTechniqueName(const std::string& name);
        const std::string& visualTechniqueNameForCameraDefault() const;
        void visualTechniqueNameForCameraDefault(const std::string& name);
        const std::string& visualTechniqueNameForCameraInside() const;
        void visualTechniqueNameForCameraInside(const std::string& name);

        const std::string& gbufferTargetName() const;
        void gbufferTargetName(const std::string& name);
        const Engine::TextureId& gbufferTextureId() const;
        void gbufferTextureId(const Engine::TextureId&);
        const std::string& gbufferSurfaceName() const;
        void gbufferSurfaceName(const std::string& name);
        const std::string& gbufferDepthName() const;
        void gbufferDepthName(const std::string& name);

        const std::vector<Graphics::RenderTextureUsage>& gbufferUsages() const;
        void gbufferUsages(const std::vector<Graphics::RenderTextureUsage>&);
        const std::vector<Graphics::GraphicFormat>& gbufferFormats() const;
        void gbufferFormats(const std::vector<Graphics::GraphicFormat>&);

        const std::string& gbufferNormalSemantic() const;
        void gbufferNormalSemantic(const std::string&);
        const std::string& gbufferDiffuseSemantic() const;
        void gbufferDiffuseSemantic(const std::string&);
        const std::string& gbufferSpecularSemantic() const;
        void gbufferSpecularSemantic(const std::string&);
        const std::string& gbufferDepthSemantic() const;
        void gbufferDepthSemantic(const std::string&);

        const SceneGraph::Spatial::SpatialFlags& sunLightSpatialFlags() const;
        void sunLightSpatialFlags(const SceneGraph::Spatial::SpatialFlags&);

        std::optional<unsigned> findRenderTextureUsageIndex(Graphics::RenderTextureUsage usage) const;

    protected:
        Engine::EffectMaterialId m_ambientEffect;
        Engine::EffectMaterialId m_sunLightEffect;
        Engine::EffectMaterialId m_lightVolumeEffect;

        std::string m_deferredRendererTechniqueName;
        std::string m_visualTechniqueNameForCameraDefault;
        std::string m_visualTechniqueNameForCameraInside;
        std::string m_gbufferTargetName;
        Engine::TextureId m_gbufferTextureId;
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

#endif // DEFERRED_RENDERING_CONFIGURATION_H

#include "DeferredRenderingConfiguration.h"
#include "GameEngine/TextureId.h"
#include <cassert>

using namespace Enigma::Rendering;
using namespace Enigma::Graphics;
using namespace Enigma::Engine;

DeferredRenderingConfiguration::DeferredRenderingConfiguration()
{
    m_ambientEffect = EffectMaterialId("fx/DeferredShadingAmbientPass");
    m_sunLightEffect = EffectMaterialId("fx/DeferredShadingSunLightPass");
    m_lightVolumeEffect = EffectMaterialId("fx/DeferredShadingLightVolume");

    m_deferredRendererTechniqueName = "DeferredRenderer";
    m_visualTechniqueNameForCameraDefault = "Default";
    m_visualTechniqueNameForCameraInside = "Inside";
    m_gbufferTargetName = "gbuffer_target";
    m_gbufferTextureId = TextureId("gbuffer_target.rnd_tex");
    m_gbufferSurfaceName = "gbuffer_surface";
    m_gbufferDepthName = "gbuffer_depth";

    m_gbufferUsages = { RenderTextureUsage::Normal, RenderTextureUsage::Albedo, RenderTextureUsage::Specular, RenderTextureUsage::Depth };
    m_gbufferFormats = { GraphicFormat::FMT_A16B16G16R16F, GraphicFormat::FMT_A16B16G16R16F, GraphicFormat::FMT_A16B16G16R16F, GraphicFormat::FMT_R32F };

    m_gbufferNormalSemantic = "GBufferNormalMap";
    m_gbufferDiffuseSemantic = "GBufferDiffuseMap";
    m_gbufferSpecularSemantic = "GBufferSpecularMap";
    m_gbufferDepthSemantic = "GBufferDepthMap";

    m_sunLightSpatialFlags = SceneGraph::Spatial::Spatial_None;
}

void DeferredRenderingConfiguration::ambientEffect(const EffectMaterialId& effect)
{
    m_ambientEffect = effect;
}

const EffectMaterialId& DeferredRenderingConfiguration::ambientEffect() const
{
    assert(!m_ambientEffect.name().empty());
    return m_ambientEffect;
}

void DeferredRenderingConfiguration::sunLightEffect(const Engine::EffectMaterialId& effect)
{
    m_sunLightEffect = effect;
}

const EffectMaterialId& DeferredRenderingConfiguration::sunLightEffect() const
{
    assert(!m_sunLightEffect.name().empty());
    return m_sunLightEffect;
}

void DeferredRenderingConfiguration::lightVolumeEffect(const Engine::EffectMaterialId& effect)
{
    m_lightVolumeEffect = effect;
}

const EffectMaterialId& DeferredRenderingConfiguration::lightVolumeEffect() const
{
    assert(!m_lightVolumeEffect.name().empty());
    return m_lightVolumeEffect;
}

void DeferredRenderingConfiguration::deferredRendererTechniqueName(const std::string& name)
{
    m_deferredRendererTechniqueName = name;
}

const std::string& DeferredRenderingConfiguration::deferredRendererTechniqueName() const
{
    assert(!m_deferredRendererTechniqueName.empty());
    return m_deferredRendererTechniqueName;
}

void DeferredRenderingConfiguration::visualTechniqueNameForCameraDefault(const std::string& name)
{
    m_visualTechniqueNameForCameraDefault = name;
}

const std::string& DeferredRenderingConfiguration::visualTechniqueNameForCameraDefault() const
{
    assert(!m_visualTechniqueNameForCameraDefault.empty());
    return m_visualTechniqueNameForCameraDefault;
}

void DeferredRenderingConfiguration::visualTechniqueNameForCameraInside(const std::string& name)
{
    m_visualTechniqueNameForCameraInside = name;

}

const std::string& DeferredRenderingConfiguration::visualTechniqueNameForCameraInside() const
{
    assert(!m_visualTechniqueNameForCameraInside.empty());
    return m_visualTechniqueNameForCameraInside;
}

void DeferredRenderingConfiguration::gbufferTargetName(const std::string& name)
{
    m_gbufferTargetName = name;
}

const std::string& DeferredRenderingConfiguration::gbufferTargetName() const
{
    assert(!m_gbufferTargetName.empty());
    return m_gbufferTargetName;
}

void DeferredRenderingConfiguration::gbufferTextureId(const Engine::TextureId& texture_id)
{
    m_gbufferTextureId = texture_id;
}

const TextureId& DeferredRenderingConfiguration::gbufferTextureId() const
{
    assert(!m_gbufferTextureId.name().empty());
    return m_gbufferTextureId;
}

void DeferredRenderingConfiguration::gbufferSurfaceName(const std::string& name)
{
    m_gbufferSurfaceName = name;
}

const std::string& DeferredRenderingConfiguration::gbufferSurfaceName() const
{
    assert(!m_gbufferSurfaceName.empty());
    return m_gbufferSurfaceName;
}

void DeferredRenderingConfiguration::gbufferDepthName(const std::string& name)
{
    m_gbufferDepthName = name;
}

const std::string& DeferredRenderingConfiguration::gbufferDepthName() const
{
    assert(!m_gbufferDepthName.empty());
    return m_gbufferDepthName;
}

void DeferredRenderingConfiguration::gbufferUsages(const std::vector<Graphics::RenderTextureUsage>& usages)
{
    m_gbufferUsages = usages;
}

const std::vector<Enigma::Graphics::RenderTextureUsage>& DeferredRenderingConfiguration::gbufferUsages() const
{
    assert(!m_gbufferUsages.empty());
    return m_gbufferUsages;
}

void DeferredRenderingConfiguration::gbufferFormats(const std::vector<Graphics::GraphicFormat>& formats)
{
    m_gbufferFormats = formats;
}

const std::vector<Enigma::Graphics::GraphicFormat>& DeferredRenderingConfiguration::gbufferFormats() const
{
    assert(!m_gbufferFormats.empty());
    return m_gbufferFormats;
}

void DeferredRenderingConfiguration::gbufferNormalSemantic(const std::string& semantic)
{
    m_gbufferNormalSemantic = semantic;
}

const std::string& DeferredRenderingConfiguration::gbufferNormalSemantic() const
{
    assert(!m_gbufferNormalSemantic.empty());
    return m_gbufferNormalSemantic;
}

void DeferredRenderingConfiguration::gbufferDiffuseSemantic(const std::string& semantic)
{
    m_gbufferDiffuseSemantic = semantic;
}

const std::string& DeferredRenderingConfiguration::gbufferDiffuseSemantic() const
{
    assert(!m_gbufferDiffuseSemantic.empty());
    return m_gbufferDiffuseSemantic;
}

void DeferredRenderingConfiguration::gbufferSpecularSemantic(const std::string& semantic)
{
    m_gbufferSpecularSemantic = semantic;
}

const std::string& DeferredRenderingConfiguration::gbufferSpecularSemantic() const
{
    assert(!m_gbufferSpecularSemantic.empty());
    return m_gbufferSpecularSemantic;
}

void DeferredRenderingConfiguration::gbufferDepthSemantic(const std::string& semantic)
{
    m_gbufferDepthSemantic = semantic;
}

const std::string& DeferredRenderingConfiguration::gbufferDepthSemantic() const
{
    assert(!m_gbufferDepthSemantic.empty());
    return m_gbufferDepthSemantic;
}

void DeferredRenderingConfiguration::sunLightSpatialFlags(const SceneGraph::Spatial::SpatialFlags& flags)
{
    m_sunLightSpatialFlags = flags;
}

const Enigma::SceneGraph::Spatial::SpatialFlags& DeferredRenderingConfiguration::sunLightSpatialFlags() const
{
    return m_sunLightSpatialFlags;
}

std::optional<unsigned> DeferredRenderingConfiguration::findRenderTextureUsageIndex(Graphics::RenderTextureUsage usage) const
{
    for (unsigned i = 0; i < m_gbufferUsages.size(); ++i)
    {
        if (m_gbufferUsages[i] == usage) return i;
    }
    return std::nullopt;
}

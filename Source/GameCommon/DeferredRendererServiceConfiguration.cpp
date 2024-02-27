#include "DeferredRendererServiceConfiguration.h"
#include "GameEngine/TextureId.h"
#include <cassert>

using namespace Enigma::GameCommon;
using namespace Enigma::Graphics;
using namespace Enigma::Engine;

DeferredRendererServiceConfiguration::DeferredRendererServiceConfiguration()
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

EffectMaterialId& DeferredRendererServiceConfiguration::ambientEffect()
{
    return m_ambientEffect;
}

const EffectMaterialId& DeferredRendererServiceConfiguration::ambientEffect() const
{
    assert(!m_ambientEffect.name().empty());
    return m_ambientEffect;
}

EffectMaterialId& DeferredRendererServiceConfiguration::sunLightEffect()
{
    return m_sunLightEffect;
}

const EffectMaterialId& DeferredRendererServiceConfiguration::sunLightEffect() const
{
    assert(!m_sunLightEffect.name().empty());
    return m_sunLightEffect;
}

EffectMaterialId& DeferredRendererServiceConfiguration::lightVolumeEffect()
{
    return m_lightVolumeEffect;
}

const EffectMaterialId& DeferredRendererServiceConfiguration::lightVolumeEffect() const
{
    assert(!m_lightVolumeEffect.name().empty());
    return m_lightVolumeEffect;
}

std::string& DeferredRendererServiceConfiguration::deferredRendererTechniqueName()
{
    return m_deferredRendererTechniqueName;
}

const std::string& DeferredRendererServiceConfiguration::deferredRendererTechniqueName() const
{
    assert(!m_deferredRendererTechniqueName.empty());
    return m_deferredRendererTechniqueName;
}

std::string& DeferredRendererServiceConfiguration::visualTechniqueNameForCameraDefault()
{
    return m_visualTechniqueNameForCameraDefault;
}

const std::string& DeferredRendererServiceConfiguration::visualTechniqueNameForCameraDefault() const
{
    assert(!m_visualTechniqueNameForCameraDefault.empty());
    return m_visualTechniqueNameForCameraDefault;
}

std::string& DeferredRendererServiceConfiguration::visualTechniqueNameForCameraInside()
{
    return m_visualTechniqueNameForCameraInside;
}

const std::string& DeferredRendererServiceConfiguration::visualTechniqueNameForCameraInside() const
{
    assert(!m_visualTechniqueNameForCameraInside.empty());
    return m_visualTechniqueNameForCameraInside;
}
std::string& DeferredRendererServiceConfiguration::gbufferTargetName()
{
    return m_gbufferTargetName;
}

const std::string& DeferredRendererServiceConfiguration::gbufferTargetName() const
{
    assert(!m_gbufferTargetName.empty());
    return m_gbufferTargetName;
}

TextureId& DeferredRendererServiceConfiguration::gbufferTextureId()
{
    return m_gbufferTextureId;
}

const TextureId& DeferredRendererServiceConfiguration::gbufferTextureId() const
{
    assert(!m_gbufferTextureId.name().empty());
    return m_gbufferTextureId;
}

std::string& DeferredRendererServiceConfiguration::gbufferSurfaceName()
{
    return m_gbufferSurfaceName;
}

const std::string& DeferredRendererServiceConfiguration::gbufferSurfaceName() const
{
    assert(!m_gbufferSurfaceName.empty());
    return m_gbufferSurfaceName;
}

std::string& DeferredRendererServiceConfiguration::gbufferDepthName()
{
    return m_gbufferDepthName;
}

const std::string& DeferredRendererServiceConfiguration::gbufferDepthName() const
{
    assert(!m_gbufferDepthName.empty());
    return m_gbufferDepthName;
}

std::vector<Enigma::Graphics::RenderTextureUsage>& DeferredRendererServiceConfiguration::gbufferUsages()
{
    return m_gbufferUsages;
}

const std::vector<Enigma::Graphics::RenderTextureUsage>& DeferredRendererServiceConfiguration::gbufferUsages() const
{
    assert(!m_gbufferUsages.empty());
    return m_gbufferUsages;
}

std::vector<Enigma::Graphics::GraphicFormat>& DeferredRendererServiceConfiguration::gbufferFormats()
{
    return m_gbufferFormats;
}

const std::vector<Enigma::Graphics::GraphicFormat>& DeferredRendererServiceConfiguration::gbufferFormats() const
{
    assert(!m_gbufferFormats.empty());
    return m_gbufferFormats;
}

std::string& DeferredRendererServiceConfiguration::gbufferNormalSemantic()
{
    return m_gbufferNormalSemantic;
}

const std::string& DeferredRendererServiceConfiguration::gbufferNormalSemantic() const
{
    assert(!m_gbufferNormalSemantic.empty());
    return m_gbufferNormalSemantic;
}

std::string& DeferredRendererServiceConfiguration::gbufferDiffuseSemantic()
{
    return m_gbufferDiffuseSemantic;
}

const std::string& DeferredRendererServiceConfiguration::gbufferDiffuseSemantic() const
{
    assert(!m_gbufferDiffuseSemantic.empty());
    return m_gbufferDiffuseSemantic;
}

std::string& DeferredRendererServiceConfiguration::gbufferSpecularSemantic()
{
    return m_gbufferSpecularSemantic;
}

const std::string& DeferredRendererServiceConfiguration::gbufferSpecularSemantic() const
{
    assert(!m_gbufferSpecularSemantic.empty());
    return m_gbufferSpecularSemantic;
}

std::string& DeferredRendererServiceConfiguration::gbufferDepthSemantic()
{
    return m_gbufferDepthSemantic;
}

const std::string& DeferredRendererServiceConfiguration::gbufferDepthSemantic() const
{
    assert(!m_gbufferDepthSemantic.empty());
    return m_gbufferDepthSemantic;
}

Enigma::SceneGraph::Spatial::SpatialFlags& DeferredRendererServiceConfiguration::sunLightSpatialFlags()
{
    return m_sunLightSpatialFlags;
}

const Enigma::SceneGraph::Spatial::SpatialFlags& DeferredRendererServiceConfiguration::sunLightSpatialFlags() const
{
    return m_sunLightSpatialFlags;
}

std::optional<unsigned> DeferredRendererServiceConfiguration::findRenderTextureUsageIndex(Graphics::RenderTextureUsage usage) const
{
    for (unsigned i = 0; i < m_gbufferUsages.size(); ++i)
    {
        if (m_gbufferUsages[i] == usage) return i;
    }
    return std::nullopt;
}

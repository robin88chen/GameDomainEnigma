#include "DeferredRendererServiceConfiguration.h"
#include <cassert>

using namespace Enigma::GameCommon;
using namespace Enigma::Graphics;

DeferredRendererServiceConfiguration::DeferredRendererServiceConfiguration()
{
    m_ambientEffectName = "DeferredShadingAmbientPass";
    m_ambientPassFxFileName = "fx/DeferredShadingAmbientPass.efx@APK_PATH";
    m_sunLightEffectName = "DeferredShadingSunLightPass";
    m_sunLightPassFxFileName = "fx/DeferredShadingSunLightPass.efx@APK_PATH";
    m_lightVolumeEffectName = "DeferredShadingLightVolume";
    m_lightVolumePassFxFileName = "fx/DeferredShadingLightVolume.efx@APK_PATH";

    m_deferredRendererTechniqueName = "DeferredRenderer";
    m_visualTechniqueNameForCameraDefault = "Default";
    m_visualTechniqueNameForCameraInside = "Inside";
    m_gbufferTargetName = "gbuffer_target";
    m_gbufferSurfaceName = "gbuffer_surface";
    m_gbufferDepthName = "gbuffer_depth";

    m_gbufferUsages = { RenderTextureUsage::Normal, RenderTextureUsage::Albedo, RenderTextureUsage::Specular, RenderTextureUsage::Depth };
    m_gbufferFormats = { GraphicFormat::FMT_A16B16G16R16F, GraphicFormat::FMT_A16B16G16R16F, GraphicFormat::FMT_A16B16G16R16F, GraphicFormat::FMT_R32F };

    m_gbufferNormalSemantic = "GBufferNormalMap";
    m_gbufferDiffuseSemantic = "GBufferDiffuseMap";
    m_gbufferSpecularSemantic = "GBufferSpecularMap";
    m_gbufferDepthSemantic = "GBufferDepthMap";
}

std::string& DeferredRendererServiceConfiguration::AmbientPassFxFileName()
{
    return m_ambientPassFxFileName;
}

const std::string& DeferredRendererServiceConfiguration::AmbientPassFxFileName() const
{
    assert(!m_ambientPassFxFileName.empty());
    return m_ambientPassFxFileName;
}

std::string& DeferredRendererServiceConfiguration::AmbientEffectName()
{
    return m_ambientEffectName;
}

const std::string& DeferredRendererServiceConfiguration::AmbientEffectName() const
{
    assert(!m_ambientEffectName.empty());
    return m_ambientEffectName;
}

std::string& DeferredRendererServiceConfiguration::SunLightPassFxFileName()
{
    return m_sunLightPassFxFileName;
}

const std::string& DeferredRendererServiceConfiguration::SunLightPassFxFileName() const
{
    assert(!m_sunLightPassFxFileName.empty());
    return m_sunLightPassFxFileName;
}

std::string& DeferredRendererServiceConfiguration::SunLightEffectName()
{
    return m_sunLightEffectName;
}

const std::string& DeferredRendererServiceConfiguration::SunLightEffectName() const
{
    assert(!m_sunLightEffectName.empty());
    return m_sunLightEffectName;
}

std::string& DeferredRendererServiceConfiguration::LightVolumePassFxFileName()
{
    return m_lightVolumePassFxFileName;
}

const std::string& DeferredRendererServiceConfiguration::LightVolumePassFxFileName() const
{
    assert(!m_lightVolumePassFxFileName.empty());
    return m_lightVolumePassFxFileName;
}

std::string& DeferredRendererServiceConfiguration::LightVolumeEffectName()
{
    return m_lightVolumeEffectName;
}

const std::string& DeferredRendererServiceConfiguration::LightVolumeEffectName() const
{
    assert(!m_lightVolumeEffectName.empty());
    return m_lightVolumeEffectName;
}

std::string& DeferredRendererServiceConfiguration::DeferredRendererTechniqueName()
{
    return m_deferredRendererTechniqueName;
}

const std::string& DeferredRendererServiceConfiguration::DeferredRendererTechniqueName() const
{
    assert(!m_deferredRendererTechniqueName.empty());
    return m_deferredRendererTechniqueName;
}

std::string& DeferredRendererServiceConfiguration::VisualTechniqueNameForCameraDefault()
{
    return m_visualTechniqueNameForCameraDefault;
}

const std::string& DeferredRendererServiceConfiguration::VisualTechniqueNameForCameraDefault() const
{
    assert(!m_visualTechniqueNameForCameraDefault.empty());
    return m_visualTechniqueNameForCameraDefault;
}

std::string& DeferredRendererServiceConfiguration::VisualTechniqueNameForCameraInside()
{
    return m_visualTechniqueNameForCameraInside;
}

const std::string& DeferredRendererServiceConfiguration::VisualTechniqueNameForCameraInside() const
{
    assert(!m_visualTechniqueNameForCameraInside.empty());
    return m_visualTechniqueNameForCameraInside;
}
std::string& DeferredRendererServiceConfiguration::GbufferTargetName()
{
    return m_gbufferTargetName;
}

const std::string& DeferredRendererServiceConfiguration::GbufferTargetName() const
{
    assert(!m_gbufferTargetName.empty());
    return m_gbufferTargetName;
}

std::string& DeferredRendererServiceConfiguration::GbufferSurfaceName()
{
    return m_gbufferSurfaceName;
}

const std::string& DeferredRendererServiceConfiguration::GbufferSurfaceName() const
{
    assert(!m_gbufferSurfaceName.empty());
    return m_gbufferSurfaceName;
}

std::string& DeferredRendererServiceConfiguration::GbufferDepthName()
{
    return m_gbufferDepthName;
}

const std::string& DeferredRendererServiceConfiguration::GbufferDepthName() const
{
    assert(!m_gbufferDepthName.empty());
    return m_gbufferDepthName;
}

std::vector<Enigma::Graphics::RenderTextureUsage>& DeferredRendererServiceConfiguration::GbufferUsages()
{
    return m_gbufferUsages;
}

const std::vector<Enigma::Graphics::RenderTextureUsage>& DeferredRendererServiceConfiguration::GbufferUsages() const
{
    assert(!m_gbufferUsages.empty());
    return m_gbufferUsages;
}

std::vector<Enigma::Graphics::GraphicFormat>& DeferredRendererServiceConfiguration::GbufferFormats()
{
    return m_gbufferFormats;
}

const std::vector<Enigma::Graphics::GraphicFormat>& DeferredRendererServiceConfiguration::GbufferFormats() const
{
    assert(!m_gbufferFormats.empty());
    return m_gbufferFormats;
}

std::string& DeferredRendererServiceConfiguration::GbufferNormalSemantic()
{
       return m_gbufferNormalSemantic;
}

const std::string& DeferredRendererServiceConfiguration::GbufferNormalSemantic() const
{
    assert(!m_gbufferNormalSemantic.empty());
    return m_gbufferNormalSemantic;
}

std::string& DeferredRendererServiceConfiguration::GbufferDiffuseSemantic()
{
   return m_gbufferDiffuseSemantic;
}

const std::string& DeferredRendererServiceConfiguration::GbufferDiffuseSemantic() const
{
    assert(!m_gbufferDiffuseSemantic.empty());
    return m_gbufferDiffuseSemantic;
}

std::string& DeferredRendererServiceConfiguration::GbufferSpecularSemantic()
{
       return m_gbufferSpecularSemantic;
}

const std::string& DeferredRendererServiceConfiguration::GbufferSpecularSemantic() const
{
       assert(!m_gbufferSpecularSemantic.empty());
    return m_gbufferSpecularSemantic;
}

std::string& DeferredRendererServiceConfiguration::GbufferDepthSemantic()
{
    return m_gbufferDepthSemantic;
}

const std::string& DeferredRendererServiceConfiguration::GbufferDepthSemantic() const
{
    assert(!m_gbufferDepthSemantic.empty());
    return m_gbufferDepthSemantic;
}

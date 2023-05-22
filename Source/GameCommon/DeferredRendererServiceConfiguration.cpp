#include "DeferredRendererServiceConfiguration.h"
#include <cassert>

using namespace Enigma::GameCommon;

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

std::string& DeferredRendererServiceConfiguration::LightVolumePassFxFileName()
{
    return m_lightVolumePassFxFileName;
}

const std::string& DeferredRendererServiceConfiguration::LightVolumePassFxFileName() const
{
    assert(!m_lightVolumePassFxFileName.empty());
    return m_lightVolumePassFxFileName;
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

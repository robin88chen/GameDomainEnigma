#include "ShadowMapServiceConfiguration.h"
#include <cassert>

using namespace Enigma::ShadowMap;

ShadowMapServiceConfiguration::ShadowMapServiceConfiguration()
{
    m_shadowMapTechniqueName = "ShadowMap";
    m_shadowMapDimension = { 512, 512 };
    m_shadowMapSurfaceName = "ShadowMapSurface";
    m_shadowMapDepthName = "ShadowMapDepth";
    m_sunLightCameraName = "SunLightCamera";
}

const std::string& ShadowMapServiceConfiguration::ShadowMapTechniqueName() const
{
    assert(!m_shadowMapTechniqueName.empty());
    return m_shadowMapTechniqueName;
}

std::string& ShadowMapServiceConfiguration::ShadowMapTechniqueName()
{
    return m_shadowMapTechniqueName;
}

const Enigma::MathLib::Dimension& ShadowMapServiceConfiguration::ShadowMapDimension() const
{
    assert(m_shadowMapDimension.m_width > 0);
    assert(m_shadowMapDimension.m_height > 0);
    return m_shadowMapDimension;
}

Enigma::MathLib::Dimension& ShadowMapServiceConfiguration::ShadowMapDimension()
{
    return m_shadowMapDimension;
}

const std::string& ShadowMapServiceConfiguration::ShadowMapSurfaceName() const
{
    assert(!m_shadowMapSurfaceName.empty());
    return m_shadowMapSurfaceName;
}

std::string& ShadowMapServiceConfiguration::ShadowMapSurfaceName()
{
    return m_shadowMapSurfaceName;
}

const std::string& ShadowMapServiceConfiguration::ShadowMapDepthName() const
{
    assert(!m_shadowMapDepthName.empty());
    return m_shadowMapDepthName;
}

std::string& ShadowMapServiceConfiguration::ShadowMapDepthName()
{
    return m_shadowMapDepthName;
}

const std::string& ShadowMapServiceConfiguration::SunLightCameraName() const
{
    assert(!m_sunLightCameraName.empty());
    return m_sunLightCameraName;
}

std::string& ShadowMapServiceConfiguration::SunLightCameraName()
{
    return m_sunLightCameraName;
}

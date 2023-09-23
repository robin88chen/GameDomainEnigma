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
    m_lightViewProjSemantic = "LightViewProj";
    m_shadowMapSemantic = "ShadowMap";
    m_shadowMapDimensionSemantic = "ShadowMapSliceDimension";
    m_shadowMapDepthBias = 0.005f;
    m_shadowMapDensity = 0.75f;
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

const Enigma::MathLib::Dimension<unsigned>& ShadowMapServiceConfiguration::ShadowMapDimension() const
{
    assert(m_shadowMapDimension.m_width > 0);
    assert(m_shadowMapDimension.m_height > 0);
    return m_shadowMapDimension;
}

Enigma::MathLib::Dimension<unsigned>& ShadowMapServiceConfiguration::ShadowMapDimension()
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

const std::string& ShadowMapServiceConfiguration::LightViewProjSemantic() const
{
    assert(!m_lightViewProjSemantic.empty());
    return m_lightViewProjSemantic;
}

std::string& ShadowMapServiceConfiguration::LightViewProjSemantic()
{
    return m_lightViewProjSemantic;
}

const std::string& ShadowMapServiceConfiguration::ShadowMapSemantic() const
{
    assert(!m_shadowMapSemantic.empty());
    return m_shadowMapSemantic;
}

std::string& ShadowMapServiceConfiguration::ShadowMapSemantic()
{
    return m_shadowMapSemantic;
}

const std::string& ShadowMapServiceConfiguration::ShadowMapDimensionSemantic() const
{
    assert(!m_shadowMapDimensionSemantic.empty());
    return m_shadowMapDimensionSemantic;
}

std::string& ShadowMapServiceConfiguration::ShadowMapDimensionSemantic()
{
    return m_shadowMapDimensionSemantic;
}

float ShadowMapServiceConfiguration::ShadowMapDepthBias() const
{
    assert(m_shadowMapDepthBias >= 0.0f);
    return m_shadowMapDepthBias;
}

float& ShadowMapServiceConfiguration::ShadowMapDepthBias()
{
       return m_shadowMapDepthBias;
}

float ShadowMapServiceConfiguration::ShadowMapDensity() const
{
    assert(m_shadowMapDensity >= 0.0f);
    return m_shadowMapDensity;
}

float& ShadowMapServiceConfiguration::ShadowMapDensity()
{
    return m_shadowMapDensity;
}

#include "ShadowMapServiceConfiguration.h"
#include "SunLightCamera.h"
#include <cassert>

using namespace Enigma::ShadowMap;

ShadowMapServiceConfiguration::ShadowMapServiceConfiguration()
{
    m_shadowMapTechniqueName = "ShadowMap";
    m_shadowMapDimension = { 512, 512 };
    m_shadowMapSurfaceName = "ShadowMapSurface";
    m_shadowMapDepthName = "ShadowMapDepth";
    m_sunLightCameraId = SceneGraph::SpatialId("SunLightCamera", SunLightCamera::TYPE_RTTI);
    m_lightViewProjSemantic = "LightViewProj";
    m_shadowMapSemantic = "ShadowMap";
    m_shadowMapDimensionSemantic = "ShadowMapSliceDimension";
    m_shadowMapDepthBias = 0.005f;
    m_shadowMapDensity = 0.75f;
}

const std::string& ShadowMapServiceConfiguration::shadowMapTechniqueName() const
{
    assert(!m_shadowMapTechniqueName.empty());
    return m_shadowMapTechniqueName;
}

std::string& ShadowMapServiceConfiguration::shadowMapTechniqueName()
{
    return m_shadowMapTechniqueName;
}

const Enigma::MathLib::Dimension<unsigned>& ShadowMapServiceConfiguration::shadowMapDimension() const
{
    assert(m_shadowMapDimension.m_width > 0);
    assert(m_shadowMapDimension.m_height > 0);
    return m_shadowMapDimension;
}

Enigma::MathLib::Dimension<unsigned>& ShadowMapServiceConfiguration::shadowMapDimension()
{
    return m_shadowMapDimension;
}

const std::string& ShadowMapServiceConfiguration::shadowMapSurfaceName() const
{
    assert(!m_shadowMapSurfaceName.empty());
    return m_shadowMapSurfaceName;
}

std::string& ShadowMapServiceConfiguration::shadowMapSurfaceName()
{
    return m_shadowMapSurfaceName;
}

const std::string& ShadowMapServiceConfiguration::shadowMapDepthName() const
{
    assert(!m_shadowMapDepthName.empty());
    return m_shadowMapDepthName;
}

std::string& ShadowMapServiceConfiguration::shadowMapDepthName()
{
    return m_shadowMapDepthName;
}

const Enigma::SceneGraph::SpatialId& ShadowMapServiceConfiguration::sunLightCameraId() const
{
    assert(!m_sunLightCameraId.name().empty());
    return m_sunLightCameraId;
}

Enigma::SceneGraph::SpatialId& ShadowMapServiceConfiguration::sunLightCameraId()
{
    return m_sunLightCameraId;
}

const std::string& ShadowMapServiceConfiguration::lightViewProjSemantic() const
{
    assert(!m_lightViewProjSemantic.empty());
    return m_lightViewProjSemantic;
}

std::string& ShadowMapServiceConfiguration::lightViewProjSemantic()
{
    return m_lightViewProjSemantic;
}

const std::string& ShadowMapServiceConfiguration::shadowMapSemantic() const
{
    assert(!m_shadowMapSemantic.empty());
    return m_shadowMapSemantic;
}

std::string& ShadowMapServiceConfiguration::shadowMapSemantic()
{
    return m_shadowMapSemantic;
}

const std::string& ShadowMapServiceConfiguration::shadowMapDimensionSemantic() const
{
    assert(!m_shadowMapDimensionSemantic.empty());
    return m_shadowMapDimensionSemantic;
}

std::string& ShadowMapServiceConfiguration::shadowMapDimensionSemantic()
{
    return m_shadowMapDimensionSemantic;
}

float ShadowMapServiceConfiguration::shadowMapDepthBias() const
{
    assert(m_shadowMapDepthBias >= 0.0f);
    return m_shadowMapDepthBias;
}

float& ShadowMapServiceConfiguration::shadowMapDepthBias()
{
    return m_shadowMapDepthBias;
}

float ShadowMapServiceConfiguration::shadowMapDensity() const
{
    assert(m_shadowMapDensity >= 0.0f);
    return m_shadowMapDensity;
}

float& ShadowMapServiceConfiguration::shadowMapDensity()
{
    return m_shadowMapDensity;
}

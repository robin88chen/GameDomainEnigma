#include "CascadeShadowMapServiceConfiguration.h"
#include "CSMSunLightCamera.h"
#include <cassert>

using namespace Enigma::ShadowMap;

CascadeShadowMapServiceConfiguration::CascadeShadowMapServiceConfiguration() : ShadowMapServiceConfiguration()
{
    m_frustaPartitionCount = 4;
    m_backfaceCullingStateName = "csm_back_cull";
    m_cascadeDistanceSemantic = "LightFrustaDistance";
    m_cascadeSliceCountSemantic = "ShadowMapSliceCount";
    m_cascadeTextureCoordTransformSemantic = "ShadowMapTexCoordTransform";
    m_sunLightCameraId = SceneGraph::SpatialId("CSMSunLightCamera", CSMSunLightCamera::TYPE_RTTI);
}

unsigned CascadeShadowMapServiceConfiguration::frustaPartitionCount() const
{
    assert(m_frustaPartitionCount > 0);
    return m_frustaPartitionCount;
}

unsigned& CascadeShadowMapServiceConfiguration::frustaPartitionCount()
{
    return m_frustaPartitionCount;
}

const std::string& CascadeShadowMapServiceConfiguration::backfaceCullingStateName() const
{
    assert(!m_backfaceCullingStateName.empty());
    return m_backfaceCullingStateName;
}

std::string& CascadeShadowMapServiceConfiguration::backfaceCullingStateName()
{
    return m_backfaceCullingStateName;
}

const std::string& CascadeShadowMapServiceConfiguration::cascadeDistanceSemantic() const
{
    assert(!m_cascadeDistanceSemantic.empty());
    return m_cascadeDistanceSemantic;
}

std::string& CascadeShadowMapServiceConfiguration::cascadeDistanceSemantic()
{
    return m_cascadeDistanceSemantic;
}

const std::string& CascadeShadowMapServiceConfiguration::cascadeTextureCoordTransformSemantic() const
{
    assert(!m_cascadeTextureCoordTransformSemantic.empty());
    return m_cascadeTextureCoordTransformSemantic;
}

std::string& CascadeShadowMapServiceConfiguration::cascadeTextureCoordTransformSemantic()
{
    return m_cascadeTextureCoordTransformSemantic;
}

const std::string& CascadeShadowMapServiceConfiguration::cascadeSliceCountSemantic() const
{
    assert(!m_cascadeSliceCountSemantic.empty());
    return m_cascadeSliceCountSemantic;
}

std::string& CascadeShadowMapServiceConfiguration::cascadeSliceCountSemantic()
{
    return m_cascadeSliceCountSemantic;
}


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

unsigned CascadeShadowMapServiceConfiguration::FrustaPartitionCount() const
{
    assert(m_frustaPartitionCount > 0);
    return m_frustaPartitionCount;
}

unsigned& CascadeShadowMapServiceConfiguration::FrustaPartitionCount()
{
    return m_frustaPartitionCount;
}

const std::string& CascadeShadowMapServiceConfiguration::BackfaceCullingStateName() const
{
    assert(!m_backfaceCullingStateName.empty());
    return m_backfaceCullingStateName;
}

std::string& CascadeShadowMapServiceConfiguration::BackfaceCullingStateName()
{
    return m_backfaceCullingStateName;
}

const std::string& CascadeShadowMapServiceConfiguration::CascadeDistanceSemantic() const
{
    assert(!m_cascadeDistanceSemantic.empty());
    return m_cascadeDistanceSemantic;
}

std::string& CascadeShadowMapServiceConfiguration::CascadeDistanceSemantic()
{
    return m_cascadeDistanceSemantic;
}

const std::string& CascadeShadowMapServiceConfiguration::CascadeTextureCoordTransformSemantic() const
{
    assert(!m_cascadeTextureCoordTransformSemantic.empty());
    return m_cascadeTextureCoordTransformSemantic;
}

std::string& CascadeShadowMapServiceConfiguration::CascadeTextureCoordTransformSemantic()
{
    return m_cascadeTextureCoordTransformSemantic;
}

const std::string& CascadeShadowMapServiceConfiguration::CascadeSliceCountSemantic() const
{
    assert(!m_cascadeSliceCountSemantic.empty());
    return m_cascadeSliceCountSemantic;
}

std::string& CascadeShadowMapServiceConfiguration::CascadeSliceCountSemantic()
{
    return m_cascadeSliceCountSemantic;
}


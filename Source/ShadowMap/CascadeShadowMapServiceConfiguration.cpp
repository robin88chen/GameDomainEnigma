#include "CascadeShadowMapServiceConfiguration.h"
#include <cassert>

using namespace Enigma::ShadowMap;

CascadeShadowMapServiceConfiguration::CascadeShadowMapServiceConfiguration() : ShadowMapServiceConfiguration()
{
    m_frustaPartitionCount = 4;
    m_backfaceCullingStateName = "csm_back_cull";
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

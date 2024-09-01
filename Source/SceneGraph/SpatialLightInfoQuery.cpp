#include "SpatialLightInfoQuery.h"

using namespace Enigma::SceneGraph;

SpatialLightInfoQuery::LightInfoDistance::LightInfoDistance()
{
    m_sqrDistance = 0.0f;
}

SpatialLightInfoQuery::LightInfoDistance::LightInfoDistance(const LightInfo& info, float sqrdist)
{
    m_lightInfo = info;
    m_sqrDistance = sqrdist;
}

SpatialLightInfoQuery::LightInfoDistance::~LightInfoDistance()
{
}

SpatialLightInfoQuery::SpatialLightInfoQuery()
{
    m_countPointLight = 0;
}

SpatialLightInfoQuery::~SpatialLightInfoQuery()
{
    m_listLightInfo.clear();
    m_countPointLight = 0;
}

void SpatialLightInfoQuery::test(const LightInfo& info)
{
    if (!info.isEnable()) return;
    if (info.lightType() == LightInfo::LightType::Unknown) return;

    if (info.lightType() == LightInfo::LightType::Point)
    {
        float length = (info.position() - m_spatialPos).length();
        if (length > info.range()) return;
        m_countPointLight++;
        m_listLightInfo.emplace_back(LightInfoDistance{ info, length });
    }
    else
    {
        m_listLightInfo.emplace_front(LightInfoDistance{ info, 0.0f });
    }
}

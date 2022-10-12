#include "SpatialRenderState.h"
#include "SpatialLightInfoQuery.h"
#include "LightInfoTraversal.h"
#include "Frameworks/ServiceManager.h"
#include "GameEngine/RenderLightingState.h"

using namespace Enigma::SceneGraph;

SpatialRenderState::SpatialRenderState()
{
}

SpatialRenderState::~SpatialRenderState()
{
}

void SpatialRenderState::QueryLightingState(const MathLib::Vector3& spatialPos)
{
    auto traversal = Frameworks::ServiceManager::GetSystemServiceAs<LightInfoTraversal>();
    SpatialLightInfoQuery query;
    query.InitSpatialPosition(spatialPos);
    traversal->QuerySpatialLightInfo(query);
    if (query.GetResultList().empty()) return;

    std::vector<MathLib::Vector4> light_positions;
    std::vector<MathLib::ColorRGBA> light_colors;
    std::vector<MathLib::Vector4> light_attenuations;

    for (SpatialLightInfoQuery::LightInfoList::const_iterator iter = query.GetResultList().begin();
        iter != query.GetResultList().end(); ++iter)
    {
        switch ((*iter).m_lightInfo.GetLightType())
        {
        case LightInfo::LightType::Ambient:
            m_lightingState.SetAmbientLightColor((*iter).m_lightInfo.GetLightColor());
            break;
        case LightInfo::LightType::SunLight:
            m_lightingState.SetSunLight((*iter).m_lightInfo.GetLightDirection(), (*iter).m_lightInfo.GetLightColor());
            break;
        case LightInfo::LightType::Point:
        {
            MathLib::Vector3 pos = (*iter).m_lightInfo.GetLightPosition();
            light_positions.emplace_back(MathLib::Vector4(pos.X(), pos.Y(), pos.Z(), (*iter).m_lightInfo.GetLightRange()));
            light_colors.emplace_back((*iter).m_lightInfo.GetLightColor());
            MathLib::Vector3 attn = (*iter).m_lightInfo.GetLightAttenuation();
            light_attenuations.emplace_back(MathLib::Vector4(attn.X(), attn.Y(), attn.Z(), 1.0f));
        }
        break;
        default:
            break;
        }
    }
    m_lightingState.SetPointLightArray(light_positions, light_colors, light_attenuations);
}

#include "GameLightCommands.h"
#include "SceneGraph/LightInfo.h"
#include "SceneGraph/SpatialId.h"
#include "SceneGraph/SceneGraphPersistenceLevel.h"
#include "GameEngine/FactoryDesc.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/Light.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;

void CreateAmbientLight::execute()
{
    assert(m_info.lightType() == LightInfo::LightType::Ambient);
    assert(m_persistenceLevel != PersistenceLevel::None);
    auto light = std::make_shared<RequestLightCreation>(m_lightId, m_info)->dispatch();
    if (!light) return;
    light->factoryDesc() = m_fd;
    if (m_persistenceLevel >= PersistenceLevel::Store)
    {
        std::make_shared<PutSpatial>(m_lightId, light)->execute();
    }
    if (!m_parentId.empty())
    {
        std::make_shared<AttachNodeChild>(m_parentId, light, MathLib::Matrix4::IDENTITY)->enqueue();
    }
}

void CreateSunLight::execute()
{
    assert(m_info.lightType() == LightInfo::LightType::SunLight);
    assert(m_persistenceLevel != PersistenceLevel::None);
    auto light = std::make_shared<RequestLightCreation>(m_lightId, m_info)->dispatch();
    if (!light) return;
    light->factoryDesc() = m_fd;
    if (m_persistenceLevel >= PersistenceLevel::Store)
    {
        std::make_shared<PutSpatial>(m_lightId, light)->execute();
    }
    if (!m_parentId.empty())
    {
        std::make_shared<AttachNodeChild>(m_parentId, light, MathLib::Matrix4::IDENTITY)->enqueue();
    }
}

void CreatePointLight::execute()
{
    assert(m_info.lightType() == LightInfo::LightType::Point);
    assert(m_persistenceLevel != PersistenceLevel::None);
    auto light = std::make_shared<RequestLightCreation>(m_lightId, m_info)->dispatch();
    if (!light) return;
    light->factoryDesc() = m_fd;
    if (m_persistenceLevel >= PersistenceLevel::Store)
    {
        std::make_shared<PutSpatial>(m_lightId, light)->execute();
    }
    if (!m_parentId.empty())
    {
        std::make_shared<AttachNodeChild>(m_parentId, light, m_mxLocal)->enqueue();
    }
}
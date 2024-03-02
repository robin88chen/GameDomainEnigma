#include "GameLightService.h"
#include "SceneGraph/LightInfo.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameLightCommands.h"
#include "GameLightEvents.h"
#include "SceneGraph/Light.h"
#include "GameSceneCommands.h"
#include "GameSceneEvents.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(GameCommon, GameLightService, ISystemService);

GameLightService::GameLightService(ServiceManager* mngr) : ISystemService(mngr)
{
    m_needTick = false;
}

ServiceResult GameLightService::onInit()
{
    m_createAmbientLight = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createAmbientLight(c); });
    CommandBus::subscribe(typeid(CreateAmbientLight), m_createAmbientLight);
    m_createSunLight = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createSunLight(c); });
    CommandBus::subscribe(typeid(CreateSunLight), m_createSunLight);
    m_createPointLight = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createPointLight(c); });
    CommandBus::subscribe(typeid(CreatePointLight), m_createPointLight);
    m_changeLightColor = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { changeLightColor(c); });
    CommandBus::subscribe(typeid(ChangeLightColor), m_changeLightColor);
    m_changeLightDirection = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { changeLightDirection(c); });
    CommandBus::subscribe(typeid(ChangeLightDirection), m_changeLightDirection);
    m_changeLightPosition = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { changeLightPosition(c); });
    CommandBus::subscribe(typeid(ChangeLightPos), m_changeLightPosition);
    m_changeLightAttenuation = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { changeLightAttenuation(c); });
    CommandBus::subscribe(typeid(ChangeLightAttenuation), m_changeLightAttenuation);
    m_changeLightRange = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { changeLightRange(c); });
    CommandBus::subscribe(typeid(ChangeLightRange), m_changeLightRange);
    m_changeLightEnable = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { changeLightAbility(c); });
    CommandBus::subscribe(typeid(EnableLight), m_changeLightEnable);
    m_changeLightDisable = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { changeLightAbility(c); });
    CommandBus::subscribe(typeid(DisableLight), m_changeLightDisable);

    m_onSceneNodeChildAttached = std::make_shared<EventSubscriber>([=](auto e) { onSceneNodeChildAttached(e); });
    EventPublisher::subscribe(typeid(SceneNodeChildAttached), m_onSceneNodeChildAttached);

    return ServiceResult::Complete;
}

ServiceResult GameLightService::onTerm()
{
    CommandBus::unsubscribe(typeid(CreateAmbientLight), m_createAmbientLight);
    m_createAmbientLight = nullptr;
    CommandBus::unsubscribe(typeid(CreateSunLight), m_createSunLight);
    m_createSunLight = nullptr;
    CommandBus::unsubscribe(typeid(CreatePointLight), m_createPointLight);
    m_createPointLight = nullptr;
    CommandBus::unsubscribe(typeid(ChangeLightColor), m_changeLightColor);
    m_changeLightColor = nullptr;
    CommandBus::unsubscribe(typeid(ChangeLightDirection), m_changeLightDirection);
    m_changeLightDirection = nullptr;
    CommandBus::unsubscribe(typeid(ChangeLightPos), m_changeLightPosition);
    m_changeLightPosition = nullptr;
    CommandBus::unsubscribe(typeid(ChangeLightAttenuation), m_changeLightAttenuation);
    m_changeLightAttenuation = nullptr;
    CommandBus::unsubscribe(typeid(ChangeLightRange), m_changeLightRange);
    m_changeLightRange = nullptr;
    CommandBus::unsubscribe(typeid(EnableLight), m_changeLightEnable);
    m_changeLightEnable = nullptr;
    CommandBus::unsubscribe(typeid(DisableLight), m_changeLightDisable);
    m_changeLightDisable = nullptr;

    EventPublisher::unsubscribe(typeid(SceneNodeChildAttached), m_onSceneNodeChildAttached);
    m_onSceneNodeChildAttached = nullptr;

    return ServiceResult::Complete;
}

void GameLightService::createAmbientLight(const SpatialId& parent_id, const SpatialId& light_id,
    const MathLib::ColorRGBA& colorLight)
{
    LightInfo info(LightInfo::LightType::Ambient);
    info.setLightColor(colorLight);
    auto light = std::make_shared<RequestLightCreation>(light_id, info, PersistenceLevel::Repository)->dispatch();
    if (parent_id.isValid())
    {
        m_pendingLightIds.insert(light_id);
        CommandBus::post(std::make_shared<AttachNodeChild>(parent_id, light, MathLib::Matrix4::IDENTITY));
    }
}

void GameLightService::createSunLight(const SpatialId& parent_id, const SpatialId& light_id,
    const MathLib::Vector3& dirLight, const MathLib::ColorRGBA& colorLight)
{
    LightInfo info(LightInfo::LightType::SunLight);
    info.setLightColor(colorLight);
    info.setLightDirection(dirLight);
    auto light = std::make_shared<RequestLightCreation>(light_id, info, PersistenceLevel::Repository)->dispatch();
    if (parent_id.isValid())
    {
        m_pendingLightIds.insert(light_id);
        CommandBus::post(std::make_shared<AttachNodeChild>(parent_id, light, MathLib::Matrix4::IDENTITY));
    }
}

void GameLightService::createPointLight(const SpatialId& parent_id, const MathLib::Matrix4& mxLocal,
    const SpatialId& light_id, const MathLib::Vector3& vecPos, const MathLib::ColorRGBA& color, float range)
{
    LightInfo info(LightInfo::LightType::Point);
    info.setLightColor(color);
    info.setLightPosition(vecPos);
    info.setLightRange(range);
    auto light = std::make_shared<RequestLightCreation>(light_id, info, PersistenceLevel::Repository)->dispatch();
    if (parent_id.isValid())
    {
        m_pendingLightIds.insert(light_id);
        CommandBus::post(std::make_shared<AttachNodeChild>(parent_id, light, mxLocal));
    }
}

void GameLightService::createAmbientLight(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateAmbientLight, ICommand>(c);
    if (!cmd) return;
    createAmbientLight(cmd->parentId(), cmd->lightId(), cmd->color());
}

void GameLightService::createSunLight(const ICommandPtr& command)
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<CreateSunLight, ICommand>(command);
    if (!cmd) return;
    createSunLight(cmd->parentId(), cmd->lightId(), cmd->direction(), cmd->color());
}

void GameLightService::createPointLight(const ICommandPtr& command)
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<CreatePointLight, ICommand>(command);
    if (!cmd) return;
    createPointLight(cmd->parentId(), cmd->localTransform(), cmd->lightId(), cmd->position(), cmd->color(), cmd->range());
}

void GameLightService::changeLightPosition(const ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<ChangeLightPos, ICommand>(command);
    if (!cmd) return;
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(cmd->lightId())->dispatch());
    if (!light) return;
    light->setLightPosition(cmd->position());
}

void GameLightService::changeLightDirection(const ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<ChangeLightDirection, ICommand>(command);
    if (!cmd) return;
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(cmd->lightId())->dispatch());
    if (!light) return;
    light->setLightDirection(cmd->direction());
}

void GameLightService::changeLightColor(const ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<ChangeLightColor, ICommand>(command);
    if (!cmd) return;
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(cmd->lightId())->dispatch());
    if (!light) return;
    light->setLightColor(cmd->color());
}

void GameLightService::changeLightAttenuation(const ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<ChangeLightAttenuation, ICommand>(command);
    if (!cmd) return;
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(cmd->lightId())->dispatch());
    if (!light) return;
    light->setLightAttenuation(MathLib::Vector3(cmd->constantFactor(), cmd->linearFactor(), cmd->quadraticFactor()));
}

void GameLightService::changeLightRange(const ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<ChangeLightRange, ICommand>(command);
    if (!cmd) return;
    const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(cmd->lightId())->dispatch());
    if (!light) return;
    light->setLightRange(cmd->range());
}

void GameLightService::changeLightAbility(const ICommandPtr& command) const
{
    if (!command) return;
    if (const auto cmd_enable = std::dynamic_pointer_cast<EnableLight, ICommand>(command))
    {
        const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(cmd_enable->lightId())->dispatch());
        if (!light) return;
        if (!light->isEnable()) light->setEnable(true);
    }
    else if (const auto cmd_disable = std::dynamic_pointer_cast<DisableLight, ICommand>(command))
    {
        const auto light = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(cmd_disable->lightId())->dispatch());
        if (!light) return;
        if (light->isEnable()) light->setEnable(false);
    }
}

void GameLightService::deleteLight(const ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<DeleteLight, ICommand>(command);
    if (!cmd) return;
    CommandBus::post(std::make_shared<DeleteSceneSpatial>(cmd->lightId()));
}

void GameLightService::onSceneNodeChildAttached(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneNodeChildAttached, IEvent>(e);
    if ((!ev) || (!ev->child())) return;
    auto light = std::dynamic_pointer_cast<Light, Spatial>(ev->child());
    if (!light) return;
    if (const auto it = m_pendingLightIds.find(light->id()); it != m_pendingLightIds.end())
    {
        EventPublisher::post(std::make_shared<GameLightCreated>(light));
        m_pendingLightIds.erase(it);
    }
}

void GameLightService::onAttachSceneNodeChildFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<AttachSceneNodeChildFailed, IEvent>(e);
    if (!ev) return;
    auto child_id = ev->childId();
    if (auto it = m_pendingLightIds.find(child_id); it != m_pendingLightIds.end())
    {
        EventPublisher::post(std::make_shared<CreateGameLightFailed>(child_id, ev->error()));
        m_pendingLightIds.erase(it);
    }
}

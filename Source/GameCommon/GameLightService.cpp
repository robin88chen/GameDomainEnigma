#include "GameLightService.h"
#include "SceneGraph/LightInfo.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameLightCommands.h"
#include "GameLightEvents.h"
#include "SceneGraph/Light.h"
#include "GameSceneCommands.h"
#include "GameSceneEvents.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(GameCommon, GameLightService, ISystemService);

GameLightService::GameLightService(Frameworks::ServiceManager* mngr,
    const std::shared_ptr<SceneGraph::SceneGraphRepository>& scene_graph_repository) : ISystemService(mngr)
{
    m_sceneGraphRepository = scene_graph_repository;
    m_needTick = false;
}

ServiceResult GameLightService::onInit()
{
    m_doCreatingAmbientLight = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingAmbientLight(c); });
    CommandBus::subscribe(typeid(GameCommon::CreateAmbientLight), m_doCreatingAmbientLight);
    m_doCreatingSunLight = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingSunLight(c); });
    CommandBus::subscribe(typeid(GameCommon::CreateSunLight), m_doCreatingSunLight);
    m_doCreatingPointLight = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingPointLight(c); });
    CommandBus::subscribe(typeid(GameCommon::CreatePointLight), m_doCreatingPointLight);
    m_doChangingLightColor = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightColor(c); });
    CommandBus::subscribe(typeid(GameCommon::ChangeLightColor), m_doChangingLightColor);
    m_doChangingLightDirection = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightDirection(c); });
    CommandBus::subscribe(typeid(GameCommon::ChangeLightDir), m_doChangingLightDirection);
    m_doChangingLightPosition = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightPosition(c); });
    CommandBus::subscribe(typeid(GameCommon::ChangeLightPos), m_doChangingLightPosition);
    m_doChangingLightAttenuation = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightAttenuation(c); });
    CommandBus::subscribe(typeid(GameCommon::ChangeLightAttenuation), m_doChangingLightAttenuation);
    m_doChangingLightRange = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightRange(c); });
    CommandBus::subscribe(typeid(GameCommon::ChangeLightRange), m_doChangingLightRange);
    m_doChangingLightEnable = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightAbility(c); });
    CommandBus::subscribe(typeid(GameCommon::EnableLight), m_doChangingLightEnable);
    m_doChangingLightDisable = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightAbility(c); });
    CommandBus::subscribe(typeid(GameCommon::DisableLight), m_doChangingLightDisable);

    m_onSceneNodeChildAttached = std::make_shared<EventSubscriber>([=](auto e) { OnSceneNodeChildAttached(e); });
    EventPublisher::subscribe(typeid(SceneNodeChildAttached), m_onSceneNodeChildAttached);

    return ServiceResult::Complete;
}

ServiceResult GameLightService::onTerm()
{
    CommandBus::unsubscribe(typeid(GameCommon::CreateAmbientLight), m_doCreatingAmbientLight);
    m_doCreatingAmbientLight = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::CreateSunLight), m_doCreatingSunLight);
    m_doCreatingSunLight = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::CreatePointLight), m_doCreatingPointLight);
    m_doCreatingPointLight = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::ChangeLightColor), m_doChangingLightColor);
    m_doChangingLightColor = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::ChangeLightDir), m_doChangingLightDirection);
    m_doChangingLightDirection = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::ChangeLightPos), m_doChangingLightPosition);
    m_doChangingLightPosition = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::ChangeLightAttenuation), m_doChangingLightAttenuation);
    m_doChangingLightAttenuation = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::ChangeLightRange), m_doChangingLightRange);
    m_doChangingLightRange = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::EnableLight), m_doChangingLightEnable);
    m_doChangingLightEnable = nullptr;
    CommandBus::unsubscribe(typeid(GameCommon::DisableLight), m_doChangingLightDisable);
    m_doChangingLightDisable = nullptr;

    EventPublisher::unsubscribe(typeid(SceneNodeChildAttached), m_onSceneNodeChildAttached);
    m_onSceneNodeChildAttached = nullptr;

    return ServiceResult::Complete;
}

void GameLightService::CreateAmbientLight(const std::string& parent_name, const std::string& lightName,
    const MathLib::ColorRGBA& colorLight)
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::Ambient);
    info.SetLightColor(colorLight);
    auto light = m_sceneGraphRepository.lock()->CreateLight(lightName, info);
    if (!parent_name.empty())
    {
        m_pendingLightNames.insert(lightName);
        CommandBus::post(std::make_shared<AttachNodeChild>(parent_name, light, MathLib::Matrix4::IDENTITY));
    }
}

void GameLightService::CreateSunLight(const std::string& parent_name, const std::string& lightName,
    const MathLib::Vector3& dirLight, const MathLib::ColorRGBA& colorLight)
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::SunLight);
    info.SetLightColor(colorLight);
    info.SetLightDirection(dirLight);
    auto light = m_sceneGraphRepository.lock()->CreateLight(lightName, info);
    if (!parent_name.empty())
    {
        m_pendingLightNames.insert(lightName);
        CommandBus::post(std::make_shared<AttachNodeChild>(parent_name, light, MathLib::Matrix4::IDENTITY));
    }
}

void GameLightService::CreatePointLight(const std::string& parent_name, const MathLib::Matrix4& mxLocal,
    const std::string& lightName, const MathLib::Vector3& vecPos, const MathLib::ColorRGBA& color, float range)
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::Point);
    info.SetLightColor(color);
    info.SetLightPosition(vecPos);
    info.SetLightRange(range);
    auto light = m_sceneGraphRepository.lock()->CreateLight(lightName, info);
    if (!parent_name.empty())
    {
        m_pendingLightNames.insert(lightName);
        CommandBus::post(std::make_shared<AttachNodeChild>(parent_name, light, mxLocal));
    }
}

void GameLightService::DoCreatingAmbientLight(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreateAmbientLight, ICommand>(c);
    if (!cmd) return;
    CreateAmbientLight(cmd->GetParentName(), cmd->GetLightName(), cmd->GetColor());
}

void GameLightService::DoCreatingSunLight(const Frameworks::ICommandPtr& command)
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreateSunLight, ICommand>(command);
    if (!cmd) return;
    CreateSunLight(cmd->GetParentName(), cmd->GetLightName(), cmd->GetDir(), cmd->GetColor());
}

void GameLightService::DoCreatingPointLight(const Frameworks::ICommandPtr& command)
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreatePointLight, ICommand>(command);
    if (!cmd) return;
    CreatePointLight(cmd->GetParentName(), cmd->getLocalTransform(), cmd->GetLightName(), cmd->GetPos(), cmd->GetColor(), cmd->GetRange());
}

void GameLightService::DoChangingLightPosition(const Frameworks::ICommandPtr& command) const
{
    assert(!m_sceneGraphRepository.expired());
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::ChangeLightPos, ICommand>(command);
    if (!cmd) return;
    const auto light = m_sceneGraphRepository.lock()->QueryLight(cmd->GetLightName());
    if (!light) return;
    light->SetLightPosition(cmd->GetPos());
}

void GameLightService::DoChangingLightDirection(const Frameworks::ICommandPtr& command) const
{
    assert(!m_sceneGraphRepository.expired());
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::ChangeLightDir, ICommand>(command);
    if (!cmd) return;
    const auto light = m_sceneGraphRepository.lock()->QueryLight(cmd->GetLightName());
    if (!light) return;
    light->SetLightDirection(cmd->GetDir());
}

void GameLightService::DoChangingLightColor(const Frameworks::ICommandPtr& command) const
{
    assert(!m_sceneGraphRepository.expired());
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::ChangeLightColor, ICommand>(command);
    if (!cmd) return;
    const auto light = m_sceneGraphRepository.lock()->QueryLight(cmd->GetLightName());
    if (!light) return;
    light->SetLightColor(cmd->GetColor());
}

void GameLightService::DoChangingLightAttenuation(const Frameworks::ICommandPtr& command) const
{
    assert(!m_sceneGraphRepository.expired());
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::ChangeLightAttenuation, ICommand>(command);
    if (!cmd) return;
    const auto light = m_sceneGraphRepository.lock()->QueryLight(cmd->GetLightName());
    if (!light) return;
    light->SetLightAttenuation(MathLib::Vector3(cmd->GetConstant(), cmd->GetLinear(), cmd->GetQuadratic()));
}

void GameLightService::DoChangingLightRange(const Frameworks::ICommandPtr& command) const
{
    assert(!m_sceneGraphRepository.expired());
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::ChangeLightRange, ICommand>(command);
    if (!cmd) return;
    const auto light = m_sceneGraphRepository.lock()->QueryLight(cmd->GetLightName());
    if (!light) return;
    light->SetLightRange(cmd->GetRange());
}

void GameLightService::DoChangingLightAbility(const Frameworks::ICommandPtr& command) const
{
    assert(!m_sceneGraphRepository.expired());
    if (!command) return;
    if (const auto cmd_enable = std::dynamic_pointer_cast<GameCommon::EnableLight, ICommand>(command))
    {
        const auto light = m_sceneGraphRepository.lock()->QueryLight(cmd_enable->GetLightName());
        if (!light) return;
        if (!light->IsEnable()) light->SetEnable(true);
    }
    else if (const auto cmd_disable = std::dynamic_pointer_cast<GameCommon::DisableLight, ICommand>(command))
    {
        const auto light = m_sceneGraphRepository.lock()->QueryLight(cmd_disable->GetLightName());
        if (!light) return;
        if (light->IsEnable()) light->SetEnable(false);
    }
}

void GameLightService::DoDeletingLight(const Frameworks::ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<DeleteLight, ICommand>(command);
    if (!cmd) return;
    CommandBus::post(std::make_shared<DeleteSceneSpatial>(cmd->GetLightName()));
}

void GameLightService::OnSceneNodeChildAttached(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneNodeChildAttached, IEvent>(e);
    if ((!ev) || (!ev->GetChild())) return;
    auto light = std::dynamic_pointer_cast<Light, Spatial>(ev->GetChild());
    if (!light) return;
    auto light_name = light->getSpatialName();
    if (const auto it = m_pendingLightNames.find(light_name); it != m_pendingLightNames.end())
    {
        EventPublisher::post(std::make_shared<GameLightCreated>(light));
        m_pendingLightNames.erase(it);
    }
}

void GameLightService::OnAttachSceneNodeChildFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<AttachSceneNodeChildFailed, IEvent>(e);
    if (!ev) return;
    auto child_name = ev->GetChildName();
    if (auto it = m_pendingLightNames.find(child_name); it != m_pendingLightNames.end())
    {
        EventPublisher::post(std::make_shared<CreateGameLightFailed>(child_name, ev->GetError()));
        m_pendingLightNames.erase(it);
    }
}

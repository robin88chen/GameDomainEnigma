#include "GameLightService.h"
#include "SceneGraph/LightInfo.h"
#include "Frameworks/CommandBus.h"
#include "GameLightCommands.h"
#include "SceneGraph/Light.h"

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

ServiceResult GameLightService::OnInit()
{
    m_doCreatingAmbientLight = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingAmbientLight(c); });
    CommandBus::Subscribe(typeid(GameCommon::CreateAmbientLight), m_doCreatingAmbientLight);
    m_doCreatingSunLight = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingSunLight(c); });
    CommandBus::Subscribe(typeid(GameCommon::CreateSunLight), m_doCreatingSunLight);
    m_doCreatingPointLight = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingPointLight(c); });
    CommandBus::Subscribe(typeid(GameCommon::CreatePointLight), m_doCreatingPointLight);
    m_doChangingLightColor = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightColor(c); });
    CommandBus::Subscribe(typeid(GameCommon::ChangeLightColor), m_doChangingLightColor);
    m_doChangingLightDirection = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightDirection(c); });
    CommandBus::Subscribe(typeid(GameCommon::ChangeLightDir), m_doChangingLightDirection);
    m_doChangingLightPosition = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightPosition(c); });
    CommandBus::Subscribe(typeid(GameCommon::ChangeLightPos), m_doChangingLightPosition);
    m_doChangingLightAttenuation = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightAttenuation(c); });
    CommandBus::Subscribe(typeid(GameCommon::ChangeLightAttenuation), m_doChangingLightAttenuation);
    m_doChangingLightRange = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightRange(c); });
    CommandBus::Subscribe(typeid(GameCommon::ChangeLightRange), m_doChangingLightRange);
    m_doChangingLightEnable = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightAbility(c); });
    CommandBus::Subscribe(typeid(GameCommon::EnableLight), m_doChangingLightEnable);
    m_doChangingLightDisable = std::make_shared<CommandSubscriber>([=](auto c) { DoChangingLightAbility(c); });
    CommandBus::Subscribe(typeid(GameCommon::DisableLight), m_doChangingLightDisable);
    return ServiceResult::Complete;
}

ServiceResult GameLightService::OnTerm()
{
    CommandBus::Unsubscribe(typeid(GameCommon::CreateAmbientLight), m_doCreatingAmbientLight);
    m_doCreatingAmbientLight = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::CreateSunLight), m_doCreatingSunLight);
    m_doCreatingSunLight = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::CreatePointLight), m_doCreatingPointLight);
    m_doCreatingPointLight = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::ChangeLightColor), m_doChangingLightColor);
    m_doChangingLightColor = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::ChangeLightDir), m_doChangingLightDirection);
    m_doChangingLightDirection = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::ChangeLightPos), m_doChangingLightPosition);
    m_doChangingLightPosition = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::ChangeLightAttenuation), m_doChangingLightAttenuation);
    m_doChangingLightAttenuation = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::ChangeLightRange), m_doChangingLightRange);
    m_doChangingLightRange = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::EnableLight), m_doChangingLightEnable);
    m_doChangingLightEnable = nullptr;
    CommandBus::Unsubscribe(typeid(GameCommon::DisableLight), m_doChangingLightDisable);
    m_doChangingLightDisable = nullptr;

    return ServiceResult::Complete;
}

void GameLightService::CreateAmbientLight(const std::shared_ptr<SceneGraph::Node>& parent,
    const std::string& lightName, const MathLib::ColorRGBA& colorLight) const
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::Ambient);
    info.SetLightColor(colorLight);
    auto light = m_sceneGraphRepository.lock()->CreateLight(lightName, info);
    if (parent) parent->AttachChild(light, MathLib::Matrix4::IDENTITY);
}

void GameLightService::CreateSunLight(const std::shared_ptr<SceneGraph::Node>& parent,
    const std::string& lightName, const MathLib::Vector3& dirLight, const MathLib::ColorRGBA& colorLight) const
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::SunLight);
    info.SetLightColor(colorLight);
    info.SetLightDirection(dirLight);
    auto light = m_sceneGraphRepository.lock()->CreateLight(lightName, info);
    if (parent) parent->AttachChild(light, MathLib::Matrix4::IDENTITY);
}

void GameLightService::CreatePointLight(const std::shared_ptr<SceneGraph::Node>& parent, const MathLib::Matrix4& mxLocal,
    const std::string& lightName, const MathLib::Vector3& vecPos, const MathLib::ColorRGBA& color, float range) const
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::Point);
    info.SetLightColor(color);
    info.SetLightPosition(vecPos);
    info.SetLightRange(range);
    auto light = m_sceneGraphRepository.lock()->CreateLight(lightName, info);
    if (parent) parent->AttachChild(light, mxLocal);
}

void GameLightService::DoCreatingAmbientLight(const Frameworks::ICommandPtr& c) const
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreateAmbientLight, ICommand>(c);
    if (!cmd) return;
    CreateAmbientLight(cmd->GetParent(), cmd->GetLightName(), cmd->GetColor());
}

void GameLightService::DoCreatingSunLight(const Frameworks::ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreateSunLight, ICommand>(command);
    if (!cmd) return;
    CreateSunLight(cmd->GetParent(), cmd->GetLightName(), cmd->GetDir(), cmd->GetColor());
}

void GameLightService::DoCreatingPointLight(const Frameworks::ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreatePointLight, ICommand>(command);
    if (!cmd) return;
    CreatePointLight(cmd->GetParent(), cmd->GetLocalTransform(), cmd->GetLightName(), cmd->GetPos(), cmd->GetColor(), cmd->GetRange());
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


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
    const std::string& lightName, const MathLib::Vector3& vecPos, const MathLib::ColorRGBA& color) const
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::Point);
    info.SetLightColor(color);
    info.SetLightPosition(vecPos);
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
    CreatePointLight(cmd->GetParent(), cmd->GetLocalTransform(), cmd->GetLightName(), cmd->GetPos(), cmd->GetColor());
}


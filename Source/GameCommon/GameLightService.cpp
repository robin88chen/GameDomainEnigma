#include "GameLightService.h"
#include "SceneGraph/LightInfo.h"
#include "Frameworks/CommandBus.h"
#include "GameLightCommands.h"

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

void GameLightService::CreateAmbientLight(const std::string& lightName, const MathLib::ColorRGBA& colorLight) const
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::Ambient);
    info.SetLightColor(colorLight);
    m_sceneGraphRepository.lock()->CreateLight(lightName, info);
}

void GameLightService::CreateSunLight(const std::string& lightName, const MathLib::Vector3& dirLight, const MathLib::ColorRGBA& colorLight) const
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::Directional);
    info.SetLightColor(colorLight);
    info.SetLightDirection(dirLight);
    m_sceneGraphRepository.lock()->CreateLight(lightName, info);
}

void GameLightService::CreatePointLight(const std::string& lightName, const MathLib::Vector3& vecPos, const MathLib::ColorRGBA& color) const
{
    assert(!m_sceneGraphRepository.expired());
    LightInfo info(LightInfo::LightType::Point);
    info.SetLightColor(color);
    info.SetLightPosition(vecPos);
    m_sceneGraphRepository.lock()->CreateLight(lightName, info);
}

void GameLightService::DoCreatingAmbientLight(const Frameworks::ICommandPtr& c) const
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreateAmbientLight, ICommand>(c);
    if (!cmd) return;
    CreateAmbientLight(cmd->GetLightName(), cmd->GetColor());
}

void GameLightService::DoCreatingSunLight(const Frameworks::ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreateSunLight, ICommand>(command);
    if (!cmd) return;
    CreateSunLight(cmd->GetLightName(), cmd->GetDir(), cmd->GetColor());
}

void GameLightService::DoCreatingPointLight(const Frameworks::ICommandPtr& command) const
{
    if (!command) return;
    const auto cmd = std::dynamic_pointer_cast<GameCommon::CreatePointLight, ICommand>(command);
    if (!cmd) return;
    CreatePointLight(cmd->GetLightName(), cmd->GetPos(), cmd->GetColor());
}


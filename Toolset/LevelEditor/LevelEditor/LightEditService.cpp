#include "LightEditService.h"
#include "Frameworks/CommandBus.h"
#include "LevelEditorCommands.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti LightEditService::TYPE_RTTI{"LevelEditor.LightEditService", &ISystemService::TYPE_RTTI};

LightEditService::LightEditService(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{

}

LightEditService::~LightEditService()
{

}

ServiceResult LightEditService::OnInit()
{
    m_doCreatingEnvironmentLight = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingEnvironmentLight(c); });
    CommandBus::Subscribe(typeid(CreateEnvironmentLight), m_doCreatingEnvironmentLight);

    return ServiceResult::Complete;
}

ServiceResult LightEditService::OnTerm()
{
    CommandBus::Unsubscribe(typeid(CreateEnvironmentLight), m_doCreatingEnvironmentLight);
    m_doCreatingEnvironmentLight = nullptr;

    return ServiceResult::Complete;
}

void LightEditService::DoCreatingEnvironmentLight(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<CreateEnvironmentLight, ICommand>(c);
    if (!cmd) return;

}

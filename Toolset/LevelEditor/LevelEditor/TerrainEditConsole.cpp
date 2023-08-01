#include "TerrainEditConsole.h"
#include "LevelEditorEvents.h"
#include "LevelEditorCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti TerrainEditConsole::TYPE_RTTI{"LevelEditor.TerrainEditConsole", &ISystemService::TYPE_RTTI};

TerrainEditConsole::TerrainEditConsole(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
    m_isEnabled = false;
}

TerrainEditConsole::~TerrainEditConsole()
{

}

ServiceResult TerrainEditConsole::OnInit()
{
    m_onEditorModeChanged = std::make_shared<EventSubscriber>([=](auto e) { OnEditorModeChanged(e); });
    EventPublisher::Subscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    return ServiceResult::Complete;
}

ServiceResult TerrainEditConsole::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onEditorModeChanged = nullptr;
    return ServiceResult::Complete;
}

void TerrainEditConsole::OnEditorModeChanged(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EditorModeChanged, IEvent>(e);
    if (!ev) return;
    m_isEnabled = ev->GetCurrMode() == EditorMode::Terrain;
    if (m_isEnabled)
    {
        CommandBus::Post(std::make_shared<OutputMessage>("Mode : Terrain Edit Selected"));
    }
}

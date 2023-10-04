#include "PawnEditConsole.h"
#include "LevelEditorEvents.h"
#include "LevelEditorCommands.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include <cassert>

using namespace LevelEditor;
using namespace Enigma::Frameworks;

Rtti PawnEditConsole::TYPE_RTTI{ "LevelEditor.PawnEditConsole", &ISystemService::TYPE_RTTI };

PawnEditConsole::PawnEditConsole(ServiceManager* srv_mngr, const std::shared_ptr<PawnEditService>& edit_service) : ISystemService(srv_mngr)
{
    m_editService = edit_service;
    m_needTick = false;
    m_isEnabled = false;
}

PawnEditConsole::~PawnEditConsole()
{
    m_pawnFullpaths.clear();
}

ServiceResult PawnEditConsole::OnInit()
{
    m_onEditorModeChanged = std::make_shared<EventSubscriber>([=](auto e) { OnEditorModeChanged(e); });
    EventPublisher::Subscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onSceneCursorPressed = std::make_shared<EventSubscriber>([=](auto e) { OnSceneCursorPressed(e); });
    EventPublisher::Subscribe(typeid(SceneCursorPressed), m_onSceneCursorPressed);
    m_onSceneCursorReleased = std::make_shared<EventSubscriber>([=](auto e) { OnSceneCursorReleased(e); });
    EventPublisher::Subscribe(typeid(SceneCursorReleased), m_onSceneCursorReleased);

    return ServiceResult::Complete;
}

ServiceResult PawnEditConsole::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onEditorModeChanged = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneCursorPressed), m_onSceneCursorPressed);
    m_onSceneCursorPressed = nullptr;
    EventPublisher::Unsubscribe(typeid(SceneCursorReleased), m_onSceneCursorReleased);
    m_onSceneCursorReleased = nullptr;

    return ServiceResult::Complete;
}

void PawnEditConsole::InsertCandidatePawnFilePath(const std::string& pawn_name, const std::string& full_path)
{
    assert(!pawn_name.empty());
    assert(!full_path.empty());

    m_pawnFullpaths.insert_or_assign(pawn_name, full_path);
}

void PawnEditConsole::SelectCandidatePawn(const std::string& pawn_name)
{
    m_selectedCandidatePawnName = pawn_name;
}

void PawnEditConsole::OnEditorModeChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EditorModeChanged>(e);
    if (!ev) return;
    m_isEnabled = ev->GetCurrMode() == EditorMode::Pawn;
    if (m_isEnabled)
    {
        CommandBus::Post(std::make_shared<OutputMessage>("Mode : Pawn Edit Selected"));
    }
}

void PawnEditConsole::OnSceneCursorPressed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneCursorPressed>(e);
    if (!ev) return;
    if (!m_isEnabled) return;

}

void PawnEditConsole::OnSceneCursorReleased(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneCursorReleased>(e);
    if (!ev) return;
    if (!m_isEnabled) return;

}

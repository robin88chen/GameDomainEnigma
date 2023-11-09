#include "PawnEditConsole.h"
#include "LevelEditorEvents.h"
#include "LevelEditorCommands.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "PawnEditService.h"
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
    m_pawnFilePaths.clear();
}

ServiceResult PawnEditConsole::onInit()
{
    m_onEditorModeChanged = std::make_shared<EventSubscriber>([=](auto e) { onEditorModeChanged(e); });
    EventPublisher::subscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onSceneCursorPressed = std::make_shared<EventSubscriber>([=](auto e) { onSceneCursorPressed(e); });
    EventPublisher::subscribe(typeid(SceneCursorPressed), m_onSceneCursorPressed);
    m_onSceneCursorReleased = std::make_shared<EventSubscriber>([=](auto e) { onSceneCursorReleased(e); });
    EventPublisher::subscribe(typeid(SceneCursorReleased), m_onSceneCursorReleased);

    return ServiceResult::Complete;
}

ServiceResult PawnEditConsole::onTerm()
{
    EventPublisher::unsubscribe(typeid(EditorModeChanged), m_onEditorModeChanged);
    m_onEditorModeChanged = nullptr;
    EventPublisher::unsubscribe(typeid(SceneCursorPressed), m_onSceneCursorPressed);
    m_onSceneCursorPressed = nullptr;
    EventPublisher::unsubscribe(typeid(SceneCursorReleased), m_onSceneCursorReleased);
    m_onSceneCursorReleased = nullptr;

    return ServiceResult::Complete;
}

void PawnEditConsole::insertCandidatePawnFilePath(const std::string& pawn_name, const std::string& filename_at_path)
{
    assert(!pawn_name.empty());
    assert(!filename_at_path.empty());

    m_pawnFilePaths.insert_or_assign(pawn_name, filename_at_path);
}

void PawnEditConsole::selectCandidatePawn(const std::string& pawn_name)
{
    m_selectedCandidatePawnName = pawn_name;
}

void PawnEditConsole::onEditorModeChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EditorModeChanged>(e);
    if (!ev) return;
    m_isEnabled = ev->currMode() == EditorMode::pawn;
    if (m_isEnabled)
    {
        CommandBus::post(std::make_shared<OutputMessage>("Mode : Pawn Edit Selected"));
    }
}

void PawnEditConsole::onSceneCursorPressed(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneCursorPressed>(e);
    if (!ev) return;
    if (!m_isEnabled) return;
    if (ev->mouseParam().m_flags.m_buttons.m_ctrl)
    {
        putSelectedPawnToScene(ev->position());
    }
}

void PawnEditConsole::onSceneCursorReleased(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<SceneCursorReleased>(e);
    if (!ev) return;
    if (!m_isEnabled) return;

}

void PawnEditConsole::putSelectedPawnToScene(const Enigma::MathLib::Vector3& pos)
{
    if (m_selectedCandidatePawnName.empty()) return;
    auto it = m_pawnFilePaths.find(m_selectedCandidatePawnName);
    if (it == m_pawnFilePaths.end()) return;
    CommandBus::post(std::make_shared<OutputMessage>("Put pawn to scene : " + it->second));
    if (!m_editService.expired()) m_editService.lock()->putCandidatePawn(m_selectedCandidatePawnName, it->second, pos);
}

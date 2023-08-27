#include "EditorSceneConsole.h"
#include "Frameworks/EventPublisher.h"
#include "InputHandlers/MouseInputEvents.h"
#include "Renderer/RendererEvents.h"

using namespace LevelEditor;
using namespace Enigma::Frameworks;
using namespace Enigma::InputHandlers;
using namespace Enigma::Renderer;

Rtti EditorSceneConsole::TYPE_RTTI{ "LevelEditor.EditorSceneConsole", &ISystemService::TYPE_RTTI };

EditorSceneConsole::EditorSceneConsole(Enigma::Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
}

EditorSceneConsole::~EditorSceneConsole()
{
}

Enigma::Frameworks::ServiceResult EditorSceneConsole::OnInit()
{
    m_onTargetViewportChanged = std::make_shared<EventSubscriber>([=](auto e) { OnTargetViewportChanged(e); });
    EventPublisher::Subscribe(typeid(TargetViewPortChanged), m_onTargetViewportChanged);
    m_onMouseMoved = std::make_shared<EventSubscriber>([=](auto e) { OnMouseMoved(e); });
    EventPublisher::Subscribe(typeid(MouseMoved), m_onMouseMoved);

    return ServiceResult::Complete;
}

Enigma::Frameworks::ServiceResult EditorSceneConsole::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(TargetViewPortChanged), m_onTargetViewportChanged);
    m_onTargetViewportChanged = nullptr;
    EventPublisher::Unsubscribe(typeid(MouseMoved), m_onMouseMoved);
    m_onMouseMoved = nullptr;

    return ServiceResult::Complete;
}

void EditorSceneConsole::OnTargetViewportChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<Enigma::Renderer::TargetViewPortChanged, IEvent>(e);
    if (!ev) return;
    m_targetViewport = ev->GetViewPort();
}

void EditorSceneConsole::OnMouseMoved(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MouseMoved>(e);
    if (!ev) return;
    auto [clipX, clipY] = m_targetViewport.ViewportXYToClippingXY(static_cast<float>(ev->m_param.m_x), static_cast<float>(ev->m_param.m_y));
}


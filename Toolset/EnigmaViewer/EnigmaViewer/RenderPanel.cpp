#include "RenderPanel.h"
#include "Renderer/RendererCommands.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "MathLib/AlgebraBasicTypes.h"
#include "Renderer/RendererEvents.h"

using namespace EnigmaViewer;
using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;

Enigma::InputHandlers::MouseKeyFlags MouseArgToKeyFlags(const nana::arg_mouse& arg)
{
    Enigma::InputHandlers::MouseKeyFlags flags;
    if (arg.left_button) flags.m_buttons.m_left = true;
    if (arg.right_button) flags.m_buttons.m_right = true;
    if (arg.mid_button) flags.m_buttons.m_middle = true;
    if (arg.shift) flags.m_buttons.m_shift = true;
    if (arg.ctrl) flags.m_buttons.m_ctrl = true;
    return flags;
}
Enigma::InputHandlers::MouseKeyFlags WheelArgToKeyFlags(const nana::arg_wheel& arg)
{
    Enigma::InputHandlers::MouseKeyFlags flags;
    if (arg.left_button) flags.m_buttons.m_left = true;
    if (arg.right_button) flags.m_buttons.m_right = true;
    if (arg.mid_button) flags.m_buttons.m_middle = true;
    if (arg.shift) flags.m_buttons.m_shift = true;
    if (arg.ctrl) flags.m_buttons.m_ctrl = true;
    flags.m_buttons.wheel_delta = static_cast<short>(arg.upwards ? 1 * static_cast<int>(arg.distance) : -1 * static_cast<int>(arg.distance));
    return flags;
}
RenderPanel::RenderPanel(nana::window wd) : nana::nested_form{ wd, nana::appear::bald<>{} }
{
    events().mouse_down([this](const nana::arg_mouse& arg) { this->OnMouseDown(arg); });
    events().mouse_up([this](const nana::arg_mouse& arg) { this->OnMouseUp(arg); });
    events().mouse_move([this](const nana::arg_mouse& arg) { this->OnMouseMove(arg); });
    events().mouse_wheel([this](const nana::arg_wheel& arg) { this->OnMouseWheel(arg); });
    events().resized([this](const nana::arg_resized& arg) { this->OnResized(arg); });
}

RenderPanel::~RenderPanel()
{
}

void RenderPanel::OnMouseDown(const nana::arg_mouse& arg)
{
    if (!m_inputHandler.expired())
    {
        if (arg.button == nana::mouse::left_button)
        {
            focus();
            m_inputHandler.lock()->ProcessWinMouseLeftButtonDown(false, (int)static_cast<short>(arg.pos.x), (int)static_cast<short>(arg.pos.y),
                MouseArgToKeyFlags(arg));
        }
        else if (arg.button == nana::mouse::right_button)
        {
            m_inputHandler.lock()->ProcessWinMouseRightButtonDown(false, (int)static_cast<short>(arg.pos.x), (int)static_cast<short>(arg.pos.y),
                MouseArgToKeyFlags(arg));
        }
        else if (arg.button == nana::mouse::middle_button)
        {
            m_inputHandler.lock()->ProcessWinMouseMiddleButtonDown(false, (int)static_cast<short>(arg.pos.x), (int)static_cast<short>(arg.pos.y),
                MouseArgToKeyFlags(arg));
        }
    }
}

void RenderPanel::OnMouseMove(const nana::arg_mouse& arg)
{
    if (!m_inputHandler.expired())
    {
        m_inputHandler.lock()->ProcessWinMouseMove((int)static_cast<short>(arg.pos.x), (int)static_cast<short>(arg.pos.y),
            MouseArgToKeyFlags(arg));
    }
}

void RenderPanel::OnMouseUp(const nana::arg_mouse& arg)
{
    if (!m_inputHandler.expired())
    {
        if (arg.button == nana::mouse::left_button)
        {
            m_inputHandler.lock()->ProcessWinMouseLeftButtonUp((int)static_cast<short>(arg.pos.x), (int)static_cast<short>(arg.pos.y),
                MouseArgToKeyFlags(arg));
        }
        else if (arg.button == nana::mouse::right_button)
        {
            m_inputHandler.lock()->ProcessWinMouseRightButtonUp((int)static_cast<short>(arg.pos.x), (int)static_cast<short>(arg.pos.y),
                MouseArgToKeyFlags(arg));
        }
        else if (arg.button == nana::mouse::middle_button)
        {
            m_inputHandler.lock()->ProcessWinMouseMiddleButtonUp((int)static_cast<short>(arg.pos.x), (int)static_cast<short>(arg.pos.y),
                MouseArgToKeyFlags(arg));
        }
    }
}

void RenderPanel::OnMouseWheel(const nana::arg_wheel& arg)
{
    if (!m_inputHandler.expired())
    {
        m_inputHandler.lock()->ProcessWinMouseWheel((int)(short)arg.pos.x, (int)(short)arg.pos.y,
            WheelArgToKeyFlags(arg));
    }
}

void RenderPanel::OnResized(const nana::arg_resized& arg)
{
    // hmm... decouple from renderer and render target, depend on more stable framework service, that's better!!
    CommandBus::Post(std::make_shared<ResizePrimaryRenderTarget>(Dimension{ arg.width, arg.height }));
}

void RenderPanel::SubscribeHandlers()
{
    m_onRenderTargetCreated = std::make_shared<EventSubscriber>([this](const IEventPtr& e) { this->OnRenderTargetCreated(e); });
    EventPublisher::Subscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
}

void RenderPanel::UnsubscribeHandlers()
{
    EventPublisher::Unsubscribe(typeid(PrimaryRenderTargetCreated), m_onRenderTargetCreated);
    m_onRenderTargetCreated = nullptr;
}

void RenderPanel::OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimaryRenderTargetCreated, IEvent>(e);
    if (!ev) return;
    CommandBus::Post(std::make_shared<ResizePrimaryRenderTarget>(Dimension{ size().width, size().height }));
    CommandBus::Post(std::make_shared<ChangeTargetViewPort>("primary_target", Enigma::Graphics::TargetViewPort{ 0, 0, size().width, size().height }));
}

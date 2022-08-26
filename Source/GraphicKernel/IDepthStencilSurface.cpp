#include "IDepthStencilSurface.h"
#include "GraphicCommands.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "Frameworks/CommandBus.h"

using namespace Enigma::Graphics;

IDepthStencilSurface::IDepthStencilSurface(const std::string& name)
    : m_name(name), m_dimension{ 0, 0 }
{
    m_doResizing =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoResizing(c); });
    Frameworks::CommandBus::Subscribe(typeid(ResizeDepthSurface), m_doResizing);
}

IDepthStencilSurface::~IDepthStencilSurface()
{
    Frameworks::CommandBus::Unsubscribe(typeid(ResizeDepthSurface), m_doResizing);
    m_doResizing = nullptr;
}

void IDepthStencilSurface::DoResizing(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ResizeDepthSurface, Frameworks::ICommand>(c);
    if (!cmd) return;
    if (cmd->GetName() != m_name) return;
    if (IGraphicAPI::Instance()->UseAsync())
    {
        AsyncResize(cmd->GetDimension());
    }
    else
    {
        Resize(cmd->GetDimension());
    }
}

future_error IDepthStencilSurface::AsyncResize(const MathLib::Dimension& dimension)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), dimension = dimension, this]() -> error { return Resize(dimension); });
}

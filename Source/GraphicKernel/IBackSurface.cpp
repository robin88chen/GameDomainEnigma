#include "IBackSurface.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "Frameworks/CommandBus.h"
#include "GraphicCommands.h"

using namespace Enigma::Graphics;

IBackSurface::IBackSurface(const std::string& name, bool primary)
    : m_name(name), m_dimension{ 0, 0 }
{
    m_isPrimary = primary;
    m_doResizing =
        std::make_shared<Frameworks::CommandSubscriber>([=](auto c) { this->DoResizing(c); });
    Frameworks::CommandBus::Subscribe(typeid(ResizeBackSurface), m_doResizing);
}

IBackSurface::~IBackSurface()
{
    Frameworks::CommandBus::Unsubscribe(typeid(ResizeBackSurface), m_doResizing);
    m_doResizing = nullptr;
}

void IBackSurface::DoResizing(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<ResizeBackSurface, Frameworks::ICommand>(c);
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

future_error IBackSurface::AsyncResize(const MathLib::Dimension& dimension)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), dimension = dimension, this]() -> error { return Resize(dimension); });
}

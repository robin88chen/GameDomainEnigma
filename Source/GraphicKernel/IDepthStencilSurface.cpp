#include "IDepthStencilSurface.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IDepthStencilSurface::IDepthStencilSurface(const std::string& name)
    : m_name(name), m_dimension{ 0, 0 }
{
}

IDepthStencilSurface::~IDepthStencilSurface()
{
}

void IDepthStencilSurface::ResizeSurface(const MathLib::Dimension<unsigned>& dimension)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        AsyncResize(dimension);
    }
    else
    {
        Resize(dimension);
    }
}

future_error IDepthStencilSurface::AsyncResize(const MathLib::Dimension<unsigned>& dimension)
{
    return IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), dimension = dimension, this]() -> error { return Resize(dimension); });
}

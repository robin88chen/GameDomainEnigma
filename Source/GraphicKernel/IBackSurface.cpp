#include "IBackSurface.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

IBackSurface::IBackSurface(const std::string& name, bool primary)
    : m_name(name), m_dimension{ 0, 0 }
{
    m_isPrimary = primary;
}

IBackSurface::~IBackSurface()
{
}

void IBackSurface::ResizeSurface(const MathLib::Dimension<unsigned>& dimension)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        AsyncResize(dimension);
    }
    else
    {
        Resize(dimension);
    }
}

future_error IBackSurface::AsyncResize(const MathLib::Dimension<unsigned>& dimension)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), dimension = dimension, this]() -> error { return Resize(dimension); });
}

#include "IBackSurface.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

future_error IBackSurface::AsyncResize(const MathLib::Dimension& dimension)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->Resize(dimension); });
}

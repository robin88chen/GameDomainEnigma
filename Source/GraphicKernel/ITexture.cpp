#include "ITexture.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"

using namespace Enigma::Graphics;

ITexture::ITexture()
{
    m_dimension = MathLib::Dimension{ 1, 1 };
    m_format = GraphicFormat::FMT_UNKNOWN;
    m_isCubeTexture = false;
}

ITexture::~ITexture()
{
}

future_error ITexture::AsyncCreateFromSystemMemory(const MathLib::Dimension& dimension, const byte_buffer& buff)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->CreateFromSystemMemory(dimension, buff); });
}

future_error ITexture::AsyncLoadTextureImage(const byte_buffer& img_buff)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->LoadTextureImage(img_buff); });
}

future_error ITexture::AsyncSaveTextureImage(const FileSystem::IFilePtr& file)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->SaveTextureImage(file); });
}

#include "ITexture.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "GraphicErrors.h"
#include "MathLib/Rect.h"
#include "FileSystem/FileSystem.h"

using namespace Enigma::Graphics;

ITexture::ITexture(const std::string& name)
{
    m_name = name;
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

future_error ITexture::AsyncRetrieveTextureImage(const MathLib::Rect& rcSrc)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->RetrieveTextureImage(rcSrc); });
}

error ITexture::LoadTextureImage(const std::string& filename, const std::string& pathid)
{
    FileSystem::IFilePtr iFile = FileSystem::FileSystem::Instance()->OpenFile(filename, "rb", pathid);
    if (!iFile) return ErrorCode::fileIO;
    size_t file_size = iFile->Size();
    if (file_size <= 0)
    {
        FileSystem::FileSystem::Instance()->CloseFile(iFile);
        return ErrorCode::fileIO;
    }
    auto buff = iFile->Read(0, file_size);
    FileSystem::FileSystem::Instance()->CloseFile(iFile);
    if (!buff) return ErrorCode::fileIO;
    if (buff.value().size() != file_size) return ErrorCode::fileIO;
    return LoadTextureImage(buff.value());
}

future_error ITexture::AsyncLoadTextureImage(const std::string& filename, const std::string& pathid)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->LoadTextureImage(filename, pathid); });
}

future_error ITexture::AsyncUpdateTextureImage(const MathLib::Rect& rcDest, const byte_buffer& img_buff)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->UpdateTextureImage(rcDest, img_buff); });
}

error ITexture::SaveTextureImage(const std::string& filename, const std::string& pathid)
{
    FileSystem::IFilePtr iFile = FileSystem::FileSystem::Instance()->OpenFile(filename, "w+b", pathid);
    if (!iFile) return ErrorCode::fileIO;
    error er = SaveTextureImage(iFile);
    FileSystem::FileSystem::Instance()->CloseFile(iFile);

    return er;
}

future_error ITexture::AsyncSaveTextureImage(const std::string& filename, const std::string& pathid)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->SaveTextureImage(filename, pathid); });
}

future_error ITexture::AsyncUseAsBackSurface(const IBackSurfacePtr& back_surf)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([=]() -> error { return shared_from_this()->UseAsBackSurface(back_surf); });
}

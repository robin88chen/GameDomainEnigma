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

void ITexture::Create(const MathLib::Dimension& dimension, const byte_buffer& buff)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), dimension, buff, this]()
                -> error { return CreateFromSystemMemory(dimension, buff); });
    }
    else
    {
        CreateFromSystemMemory(dimension, buff);
    }
}

void ITexture::Load(const byte_buffer& img_buff)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), img_buff, this]() -> error { return LoadTextureImage(img_buff); });
    }
    else
    {
        LoadTextureImage(img_buff);
    }
}

void ITexture::Load(const std::string& filename, const std::string& pathid)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filename, pathid, this]() -> error { return LoadTextureImage(filename, pathid); });
    }
    else
    {
        LoadTextureImage(filename, pathid);
    }
}

void ITexture::Save(const FileSystem::IFilePtr& file)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), file, this]() -> error { return SaveTextureImage(file); });
    }
    else
    {
        SaveTextureImage(file);
    }
}

void ITexture::Save(const std::string& filename, const std::string& pathid)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filename, pathid, this]()
                -> error { return SaveTextureImage(filename, pathid); });
    }
    else
    {
        SaveTextureImage(filename, pathid);
    }
}

void ITexture::Retrieve(const MathLib::Rect& rcSrc)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), rcSrc, this]() -> error { return RetrieveTextureImage(rcSrc); });
    }
    else
    {
        RetrieveTextureImage(rcSrc);
    }
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

void ITexture::Update(const MathLib::Rect& rcDest, const byte_buffer& img_buff)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), rcDest, img_buff, this]() 
                -> error { return UpdateTextureImage(rcDest, img_buff); });
    }
    else
    {
        UpdateTextureImage(rcDest, img_buff);
    }
}

error ITexture::SaveTextureImage(const std::string& filename, const std::string& pathid)
{
    FileSystem::IFilePtr iFile = FileSystem::FileSystem::Instance()->OpenFile(filename, "w+b", pathid);
    if (!iFile) return ErrorCode::fileIO;
    error er = SaveTextureImage(iFile);
    FileSystem::FileSystem::Instance()->CloseFile(iFile);

    return er;
}

void ITexture::AsBackSurface(const IBackSurfacePtr& back_surf)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), back_surf, this]() -> error { return UseAsBackSurface(back_surf); });
    }
    else
    {
        UseAsBackSurface(back_surf);
    }
}

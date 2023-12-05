#include "ITexture.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "GraphicErrors.h"
#include "MathLib/Rect.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/Filename.h"

using namespace Enigma::Graphics;

ITexture::ITexture(const std::string& name) : m_dimension{ 1, 1 }
{
    m_name = name;
    m_format = GraphicFormat::FMT_UNKNOWN;
    m_isCubeTexture = false;
}

ITexture::~ITexture()
{
}

void ITexture::create(const MathLib::Dimension<unsigned>& dimension, const byte_buffer& buff)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
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
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), img_buff, this]() -> error { return LoadTextureImage(img_buff); });
    }
    else
    {
        LoadTextureImage(img_buff);
    }
}

void ITexture::Load(const std::string& filename, const std::string& pathid)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filename, pathid, this]() -> error { return LoadTextureImage(filename, pathid); });
    }
    else
    {
        LoadTextureImage(filename, pathid);
    }
}

void ITexture::Save(const FileSystem::IFilePtr& file)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), file, this]() -> error { return SaveTextureImage(file); });
    }
    else
    {
        SaveTextureImage(file);
    }
}

void ITexture::Save(const std::string& filename, const std::string& pathid)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
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
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), rcSrc, this]() -> error { return RetrieveTextureImage(rcSrc); });
    }
    else
    {
        RetrieveTextureImage(rcSrc);
    }
}

error ITexture::LoadTextureImage(const std::string& filename, const std::string& pathid)
{
    FileSystem::Filename filename_at_path(filename, pathid);
    FileSystem::IFilePtr iFile = FileSystem::FileSystem::instance()->openFile(filename_at_path.getSubPathFileName(), FileSystem::read | FileSystem::binary, filename_at_path.getMountPathId());
    if (!iFile) return ErrorCode::fileIO;
    size_t file_size = iFile->size();
    if (file_size <= 0)
    {
        FileSystem::FileSystem::instance()->closeFile(iFile);
        return ErrorCode::fileIO;
    }
    auto buff = iFile->read(0, file_size);
    FileSystem::FileSystem::instance()->closeFile(iFile);
    if (!buff) return ErrorCode::fileIO;
    if (buff.value().size() != file_size) return ErrorCode::fileIO;
    return LoadTextureImage(buff.value());
}

void ITexture::update(const MathLib::Rect& rcDest, const byte_buffer& img_buff)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
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
    FileSystem::Filename filename_at_path(filename, pathid);
    FileSystem::IFilePtr iFile = FileSystem::FileSystem::instance()->openFile(filename_at_path.getSubPathFileName(), FileSystem::write | FileSystem::openAlways | FileSystem::binary, filename_at_path.getMountPathId());
    if (!iFile) return ErrorCode::fileIO;
    error er = SaveTextureImage(iFile);
    FileSystem::FileSystem::instance()->closeFile(iFile);

    return er;
}

void ITexture::AsBackSurface(const IBackSurfacePtr& back_surf, const std::vector<RenderTextureUsage>& usages)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), back_surf, usages, this]() -> error { return UseAsBackSurface(back_surf, usages); });
    }
    else
    {
        UseAsBackSurface(back_surf, usages);
    }
}

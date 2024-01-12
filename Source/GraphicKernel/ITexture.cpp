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
                -> error { return createFromSystemMemory(dimension, buff); });
    }
    else
    {
        createFromSystemMemory(dimension, buff);
    }
}

void ITexture::load(const byte_buffer& img_buff)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), img_buff, this]() -> error { return loadTextureImage(img_buff); });
    }
    else
    {
        loadTextureImage(img_buff);
    }
}

void ITexture::load(const std::string& filename, const std::string& pathid)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filename, pathid, this]() -> error { return loadTextureImage(filename, pathid); });
    }
    else
    {
        loadTextureImage(filename, pathid);
    }
}

void ITexture::save(const FileSystem::IFilePtr& file)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), file, this]() -> error { return saveTextureImage(file); });
    }
    else
    {
        saveTextureImage(file);
    }
}

void ITexture::save(const std::string& filename, const std::string& pathid)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filename, pathid, this]()
                -> error { return saveTextureImage(filename, pathid); });
    }
    else
    {
        saveTextureImage(filename, pathid);
    }
}

void ITexture::retrieve(const MathLib::Rect& rcSrc)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), rcSrc, this]() -> error { return retrieveTextureImage(rcSrc); });
    }
    else
    {
        retrieveTextureImage(rcSrc);
    }
}

error ITexture::loadTextureImage(const std::string& filename, const std::string& pathid)
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
    return loadTextureImage(buff.value());
}

void ITexture::update(const MathLib::Rect& rcDest, const byte_buffer& img_buff)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), rcDest, img_buff, this]()
                -> error { return updateTextureImage(rcDest, img_buff); });
    }
    else
    {
        updateTextureImage(rcDest, img_buff);
    }
}

error ITexture::saveTextureImage(const std::string& filename, const std::string& pathid)
{
    FileSystem::Filename filename_at_path(filename, pathid);
    FileSystem::IFilePtr iFile = FileSystem::FileSystem::instance()->openFile(filename_at_path.getSubPathFileName(), FileSystem::write | FileSystem::openAlways | FileSystem::binary, filename_at_path.getMountPathId());
    if (!iFile) return ErrorCode::fileIO;
    error er = saveTextureImage(iFile);
    FileSystem::FileSystem::instance()->closeFile(iFile);

    return er;
}

void ITexture::asBackSurface(const IBackSurfacePtr& back_surf, const std::vector<RenderTextureUsage>& usages)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), back_surf, usages, this]() -> error { return useAsBackSurface(back_surf, usages); });
    }
    else
    {
        useAsBackSurface(back_surf, usages);
    }
}

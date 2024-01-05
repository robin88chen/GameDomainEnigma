#include "IMultiTexture.h"
#include "IGraphicAPI.h"
#include "GraphicThread.h"
#include "FileSystem/FileSystem.h"

using namespace Enigma::Graphics;

IMultiTexture::IMultiTexture(const std::string& name) : ITexture(name)
{
}

IMultiTexture::~IMultiTexture()
{
}

void IMultiTexture::multiCreate(const MathLib::Dimension<unsigned>& dimension, unsigned count, const std::vector<byte_buffer>& buffs)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), dimension, count, buffs, this]()
                -> error { return createFromSystemMemories(dimension, count, buffs); });
    }
    else
    {
        createFromSystemMemories(dimension, count, buffs);
    }
}

void IMultiTexture::multiLoad(const std::vector<byte_buffer>& img_buffs)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), img_buffs, this]() -> error { return loadTextureImages(img_buffs); });
    }
    else
    {
        loadTextureImages(img_buffs);
    }
}

void IMultiTexture::multiLoad(const std::vector<std::string>& filenames, const std::vector<std::string>& pathids)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filenames, pathids, this]() -> error { return loadTextureImages(filenames, pathids); });
    }
    else
    {
        loadTextureImages(filenames, pathids);
    }
}

void IMultiTexture::multiSave(const std::vector<FileSystem::IFilePtr>& files)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), files, this]() -> error { return saveTextureImages(files); });
    }
    else
    {
        saveTextureImages(files);
    }
}

void IMultiTexture::multiSave(const std::vector<std::string>& filenames, const std::vector<std::string>& pathids)
{
    if (IGraphicAPI::instance()->UseAsync())
    {
        IGraphicAPI::instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filenames, pathids, this]()
                -> error { return saveTextureImages(filenames, pathids); });
    }
    else
    {
        saveTextureImages(filenames, pathids);
    }
}


future_error IMultiTexture::asyncLoadTextureImages(const std::vector<byte_buffer>& img_buffs)
{
    return IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), img_buffs, this]() -> error { return loadTextureImages(img_buffs); });
}


future_error IMultiTexture::asyncSaveTextureImages(const std::vector<FileSystem::IFilePtr>& files)
{
    return IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), files, this]() -> error { return saveTextureImages(files); });
}

error IMultiTexture::loadTextureImages(const std::vector<std::string>& filenames,
    const std::vector<std::string>& pathids)
{
    size_t count = filenames.size();
    if (count > pathids.size()) count = pathids.size();
    std::vector<byte_buffer> buffs;
    buffs.resize(count);
    for (size_t i = 0; i < count; i++)
    {
        FileSystem::IFilePtr iFile = FileSystem::FileSystem::instance()->openFile(filenames[i], FileSystem::read | FileSystem::binary, pathids[i]);
        if (!iFile) return ErrorCode::fileIO;
        size_t file_size = iFile->size();
        if (file_size <= 0)
        {
            FileSystem::FileSystem::instance()->closeFile(iFile);
            return ErrorCode::fileIO;
        }
        auto reading = iFile->read(0, file_size);
        if (reading) buffs[i] = reading.value();
        size_t read_bytes = buffs[i].size();
        FileSystem::FileSystem::instance()->closeFile(iFile);
        if (read_bytes != file_size) return ErrorCode::fileIO;
    }
    return loadTextureImages(buffs);
}

future_error IMultiTexture::asyncLoadTextureImages(const std::vector<std::string>& filenames,
    const std::vector<std::string>& pathids)
{
    return IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), filenames, pathids, this]() -> error { return loadTextureImages(filenames, pathids); });
}

error IMultiTexture::saveTextureImages(const std::vector<std::string>& filenames,
    const std::vector<std::string>& pathids)
{
    size_t count = filenames.size();
    if (count > pathids.size()) count = pathids.size();
    std::vector<FileSystem::IFilePtr> files;
    files.resize(count);
    for (size_t i = 0; i < count; i++)
    {
        FileSystem::IFilePtr iFile = FileSystem::FileSystem::instance()->openFile(filenames[i], FileSystem::write | FileSystem::openAlways | FileSystem::binary, pathids[i]);
        if (!iFile) return ErrorCode::fileIO;
        files.emplace_back(iFile);
    }
    error er = saveTextureImages(files);
    for (auto file : files)
    {
        FileSystem::FileSystem::instance()->closeFile(file);
    }
    return er;
}

future_error IMultiTexture::asyncSaveTextureImages(const std::vector<std::string>& filenames,
    const std::vector<std::string>& pathids)
{
    return IGraphicAPI::instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), filenames, pathids, this]() -> error { return saveTextureImages(filenames, pathids); });
}

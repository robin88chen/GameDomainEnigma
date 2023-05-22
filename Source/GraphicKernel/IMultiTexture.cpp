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

void IMultiTexture::MultiCreate(const MathLib::Dimension& dimension, unsigned count, const std::vector<byte_buffer>& buffs)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), dimension, count, buffs, this]()
                -> error { return CreateFromSystemMemories(dimension, count, buffs); });
    }
    else
    {
        CreateFromSystemMemories(dimension, count, buffs);
    }
}

void IMultiTexture::MultiLoad(const std::vector<byte_buffer>& img_buffs)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), img_buffs, this]() -> error { return LoadTextureImages(img_buffs); });
    }
    else
    {
        LoadTextureImages(img_buffs);
    }
}

void IMultiTexture::MultiLoad(const std::vector<std::string>& filenames, const std::vector<std::string>& pathids)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filenames, pathids, this]() -> error { return LoadTextureImages(filenames, pathids); });
    }
    else
    {
        LoadTextureImages(filenames, pathids);
    }
}

void IMultiTexture::MultiSave(const std::vector<FileSystem::IFilePtr>& files)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), files, this]() -> error { return SaveTextureImages(files); });
    }
    else
    {
        SaveTextureImages(files);
    }
}

void IMultiTexture::MultiSave(const std::vector<std::string>& filenames, const std::vector<std::string>& pathids)
{
    if (IGraphicAPI::Instance()->UseAsync())
    {
        IGraphicAPI::Instance()->GetGraphicThread()->
            PushTask([lifetime = shared_from_this(), filenames, pathids, this]()
                -> error { return SaveTextureImages(filenames, pathids); });
    }
    else
    {
        SaveTextureImages(filenames, pathids);
    }
}


future_error IMultiTexture::AsyncLoadTextureImages(const std::vector<byte_buffer>& img_buffs)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), img_buffs, this]() -> error { return LoadTextureImages(img_buffs); });
}


future_error IMultiTexture::AsyncSaveTextureImages(const std::vector<FileSystem::IFilePtr>& files)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), files, this]() -> error { return SaveTextureImages(files); });
}

error IMultiTexture::LoadTextureImages(const std::vector<std::string>& filenames,
    const std::vector<std::string>& pathids)
{
    size_t count = filenames.size();
    if (count > pathids.size()) count = pathids.size();
    std::vector<byte_buffer> buffs;
    buffs.resize(count);
    for (size_t i = 0; i < count; i++)
    {
        FileSystem::IFilePtr iFile = FileSystem::FileSystem::Instance()->OpenFile(filenames[i], "rb", pathids[i]);
        if (!iFile) return ErrorCode::fileIO;
        size_t file_size = iFile->Size();
        if (file_size <= 0)
        {
            FileSystem::FileSystem::Instance()->CloseFile(iFile);
            return ErrorCode::fileIO;
        }
        auto reading = iFile->Read(0, file_size);
        if (reading) buffs[i] = reading.value();
        size_t read_bytes = buffs[i].size();
        FileSystem::FileSystem::Instance()->CloseFile(iFile);
        if (read_bytes != file_size) return ErrorCode::fileIO;
    }
    return LoadTextureImages(buffs);
}

future_error IMultiTexture::AsyncLoadTextureImages(const std::vector<std::string>& filenames,
                                                   const std::vector<std::string>& pathids)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), filenames, pathids, this]() -> error { return LoadTextureImages(filenames, pathids); });
}

error IMultiTexture::SaveTextureImages(const std::vector<std::string>& filenames,
    const std::vector<std::string>& pathids)
{
    size_t count = filenames.size();
    if (count > pathids.size()) count = pathids.size();
    std::vector<FileSystem::IFilePtr> files;
    files.resize(count);
    for (size_t i = 0; i < count; i++)
    {
        FileSystem::IFilePtr iFile = FileSystem::FileSystem::Instance()->OpenFile(filenames[i], "w+b", pathids[i]);
        if (!iFile) return ErrorCode::fileIO;
        files.emplace_back(iFile);
    }
    error er = SaveTextureImages(files);
    for (auto file : files)
    {
        FileSystem::FileSystem::Instance()->CloseFile(file);
    }
    return er;
}

future_error IMultiTexture::AsyncSaveTextureImages(const std::vector<std::string>& filenames,
    const std::vector<std::string>& pathids)
{
    return IGraphicAPI::Instance()->GetGraphicThread()->
        PushTask([lifetime = shared_from_this(), filenames, pathids, this]() -> error { return SaveTextureImages(filenames, pathids); });
}

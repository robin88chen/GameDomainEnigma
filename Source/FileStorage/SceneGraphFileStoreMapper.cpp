#include "SceneGraphFileStoreMapper.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileSystemErrors.h"
#include <cassert>

using namespace Enigma::FileStorage;

SceneGraphFileStoreMapper::SceneGraphFileStoreMapper(const std::string& mapper_filename) : SceneGraph::SceneGraphStoreMapper()
{
    m_mapper_filename = mapper_filename;
}

SceneGraphFileStoreMapper::~SceneGraphFileStoreMapper()
{
    assert(m_filename_map.empty());
}

std::error_code SceneGraphFileStoreMapper::connect()
{
    m_filename_map.clear();
    FileSystem::IFilePtr mapper_file = FileSystem::FileSystem::instance()->openFile(m_mapper_filename, FileSystem::ReadWriteOptionRead);
    if (!mapper_file) return FileSystem::ErrorCode::ok;
    auto file_size = mapper_file->size();
    auto content = mapper_file->read(0, file_size);
    if (!content) return FileSystem::ErrorCode::readFail;
    parseMapperFile(std::string(content->begin(), content->end()));
    return FileSystem::ErrorCode::ok;
}

std::error_code SceneGraphFileStoreMapper::disconnect()
{
    m_filename_map.clear();
    return FileSystem::ErrorCode::ok;
}


#include "SceneGraphFileStoreMapper.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileSystemErrors.h"
#include "SceneGraph/SceneGraphErrors.h"
#include "SceneGraph/Camera.h"
#include <cassert>

using namespace Enigma::FileStorage;

SceneGraphFileStoreMapper::SceneGraphFileStoreMapper(const std::string& mapper_filename) : SceneGraph::SceneGraphStoreMapper()
{
    m_mapper_filename = mapper_filename;
    m_has_connected = false;
}

SceneGraphFileStoreMapper::~SceneGraphFileStoreMapper()
{
    assert(m_filename_map.empty());
}

std::error_code SceneGraphFileStoreMapper::connect()
{
    if (m_has_connected) return FileSystem::ErrorCode::ok;
    m_filename_map.clear();
    FileSystem::IFilePtr mapper_file = FileSystem::FileSystem::instance()->openFile(m_mapper_filename, FileSystem::ReadWriteOptionRead);
    if (!mapper_file)
    {
        m_has_connected = true;
        return FileSystem::ErrorCode::ok;
    }
    auto file_size = mapper_file->size();
    auto content = mapper_file->read(0, file_size);
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (!content) return FileSystem::ErrorCode::readFail;
    deserializeMapperFile(std::string(content->begin(), content->end()));
    m_has_connected = true;
    return FileSystem::ErrorCode::ok;
}

std::error_code SceneGraphFileStoreMapper::disconnect()
{
    m_filename_map.clear();
    m_has_connected = false;
    return FileSystem::ErrorCode::ok;
}

bool SceneGraphFileStoreMapper::hasCamera(const SceneGraph::SpatialId& id)
{
    assert(id.rtti().isDerived(SceneGraph::Camera::TYPE_RTTI));
    if (!m_has_connected) connect();
    return m_filename_map.find(id) != m_filename_map.end();
}

std::shared_ptr<Enigma::SceneGraph::Camera> SceneGraphFileStoreMapper::queryCamera(const SceneGraph::SpatialId& id)
{
    return nullptr;
}

std::error_code SceneGraphFileStoreMapper::removeCamera(const SceneGraph::SpatialId& id)
{
    assert(id.rtti().isDerived(SceneGraph::Camera::TYPE_RTTI));
    m_filename_map.erase(id);
    auto content = serializeMapperFile();
    return SceneGraph::ErrorCode::ok;
}

std::error_code SceneGraphFileStoreMapper::putCamera(const SceneGraph::SpatialId& id, const std::shared_ptr<SceneGraph::Camera>& camera)
{
    auto filename = extractFilename(id, camera->factoryDesc());
    m_filename_map.insert_or_assign(id, filename);
    auto content = serializeMapperFile();
    return SceneGraph::ErrorCode::ok;
}

std::string SceneGraphFileStoreMapper::serializeMapperFile()
{
    return "";
}
void SceneGraphFileStoreMapper::deserializeMapperFile(const std::string& mapper_file_content)
{

}

std::string SceneGraphFileStoreMapper::extractFilename(const SceneGraph::SpatialId& id, const Engine::FactoryDesc& factory_desc)
{
    if (!factory_desc.GetDeferredFilename().empty()) return factory_desc.GetDeferredFilename();
    if (!factory_desc.GetResourceFilename().empty()) return factory_desc.GetResourceFilename();
    return id.name() + "." + factory_desc.GetRttiName() + ".json";
}

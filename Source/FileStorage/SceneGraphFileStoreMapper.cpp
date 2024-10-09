#include "SceneGraphFileStoreMapper.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileSystemErrors.h"
#include "SceneGraph/SceneGraphErrors.h"
#include "SceneGraph/Camera.h"
#include "Frameworks/TokenVector.h"
#include "SceneGraph/Spatial.h"
#include <cassert>

using namespace Enigma::FileStorage;

SceneGraphFileStoreMapper::SpatialFileMap::SpatialFileMap(const std::string& filename, const std::string& asset_prefix, const std::shared_ptr<Gateways::IDtoGateway>& gateway)
{
    m_gateway = gateway;
    m_filename = filename;
    m_assetPrefix = asset_prefix;
}

SceneGraphFileStoreMapper::SpatialFileMap::~SpatialFileMap()
{
    assert(m_map.empty());
}

std::error_code SceneGraphFileStoreMapper::SpatialFileMap::connect()
{
    std::lock_guard locker{ m_lock };
    m_map.clear();
    FileSystem::IFilePtr mapper_file = FileSystem::FileSystem::instance()->openFile(m_filename, FileSystem::read | FileSystem::binary);
    if (!mapper_file) return FileSystem::ErrorCode::ok;
    auto file_size = mapper_file->size();
    if (file_size == 0) return FileSystem::ErrorCode::ok;
    auto content = mapper_file->read(0, file_size);
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (!content) return FileSystem::ErrorCode::readFail;
    const std::string mapper_content{ content->begin(), content->end() };
    deserializeMapperFile(mapper_content);
    return FileSystem::ErrorCode::ok;
}

std::error_code SceneGraphFileStoreMapper::SpatialFileMap::disconnect()
{
    std::lock_guard locker{ m_lock };
    m_map.clear();
    return FileSystem::ErrorCode::ok;
}

bool SceneGraphFileStoreMapper::SpatialFileMap::has(const SceneGraph::SpatialId& id)
{
    std::lock_guard locker{ m_lock };
    return m_map.find(id) != m_map.end();
}

std::optional<Enigma::Engine::GenericDto> SceneGraphFileStoreMapper::SpatialFileMap::query(const SceneGraph::SpatialId& id)
{
    std::lock_guard locker{ m_lock };
    auto it = m_map.find(id);
    if (it == m_map.end()) return std::nullopt;
    return deserializeDataTransferObjects(it->second);
}

std::error_code SceneGraphFileStoreMapper::SpatialFileMap::remove(const SceneGraph::SpatialId& id)
{
    std::lock_guard locker{ m_lock };
    m_map.erase(id);
    auto er = serializeMapperFile();
    if (er) return er;
    return SceneGraph::ErrorCode::ok;
}

std::error_code SceneGraphFileStoreMapper::SpatialFileMap::put(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto)
{
    auto filename = extractFilename(id, dto.getRtti());
    std::lock_guard locker{ m_lock };
    m_map.insert_or_assign(id, filename);
    auto er = serializeMapperFile();
    if (er) return er;
    er = serializeDataTransferObjects(filename, dto);
    if (er) return er;
    return SceneGraph::ErrorCode::ok;
}

void SceneGraphFileStoreMapper::SpatialFileMap::deserializeMapperFile(const std::string& content)
{
    if (content.empty()) return;
    auto lines = split_token(content, "\n\r");
    for (auto& line : lines)
    {
        auto tokens = split_token(line, ",");
        if (tokens.size() != 3) continue;
        SceneGraph::SpatialId id{ tokens[0], Frameworks::Rtti::fromName(tokens[1]) };
        m_map.insert_or_assign(id, tokens[2]);
    }
}

Enigma::Engine::GenericDto SceneGraphFileStoreMapper::SpatialFileMap::deserializeDataTransferObjects(const std::string& filename)
{
    assert(m_gateway);
    auto dto_file = FileSystem::FileSystem::instance()->openFile(filename, FileSystem::read | FileSystem::binary);
    auto file_size = dto_file->size();
    auto content = dto_file->read(0, file_size);
    assert(content.has_value());
    FileSystem::FileSystem::instance()->closeFile(dto_file);
    const auto dtos = m_gateway->deserialize(std::string(content.value().begin(), content.value().end()));
    assert(!dtos.empty());
    return dtos[0];
}

std::error_code SceneGraphFileStoreMapper::SpatialFileMap::serializeMapperFile()
{
    std::lock_guard locker{ m_lock };
    std::string mapper_file_content;
    for (auto& rec : m_map)
    {
        mapper_file_content += rec.first.name() + "," + rec.first.rtti().getName() + "," + rec.second + "\n";
    }
    auto mapper_file = FileSystem::FileSystem::instance()->openFile(m_filename, FileSystem::write | FileSystem::binary);
    if (!mapper_file) return FileSystem::ErrorCode::fileOpenError;
    auto write_size = mapper_file->write(0, { mapper_file_content.begin(), mapper_file_content.end() });
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (write_size != mapper_file_content.size()) return FileSystem::ErrorCode::writeFail;
    return FileSystem::ErrorCode::ok;
}

std::string SceneGraphFileStoreMapper::SpatialFileMap::extractFilename(const SceneGraph::SpatialId& id, const Engine::FactoryDesc& factory_desc)
{
    if (m_assetPrefix.empty())
    {
        if (!factory_desc.deferredFilename().empty()) return factory_desc.deferredFilename();
        if (!factory_desc.resourceFilename().empty()) return factory_desc.resourceFilename();
        return id.name() + "." + factory_desc.rttiName() + ".json";
    }
    std::string filename;
    if (!factory_desc.deferredFilename().empty())
    {
        filename = factory_desc.deferredFilename();
    }
    else if (!factory_desc.resourceFilename().empty())
    {
        filename = factory_desc.resourceFilename();
    }
    else
    {
        filename = id.name() + "." + factory_desc.rttiName() + ".json";
    }
    auto pos = filename.find_last_of('/');
    if (pos == std::string::npos) return m_assetPrefix + filename;
    return filename.insert(pos + 1, m_assetPrefix);
}

std::error_code SceneGraphFileStoreMapper::SpatialFileMap::serializeDataTransferObjects(const std::string& filename, const Engine::GenericDto& dto)
{
    assert(m_gateway);
    FileSystem::IFilePtr dto_file = FileSystem::FileSystem::instance()->openFile(filename, FileSystem::write | FileSystem::binary);
    if (!dto_file) return FileSystem::ErrorCode::fileOpenError;
    auto content = m_gateway->serialize({ dto });
    auto write_size = dto_file->write(0, { content.begin(), content.end() });
    FileSystem::FileSystem::instance()->closeFile(dto_file);
    if (write_size != content.size()) return FileSystem::ErrorCode::writeFail;
    return FileSystem::ErrorCode::ok;
}

SceneGraphFileStoreMapper::SceneGraphFileStoreMapper(const std::string& mapper_filename, const std::string& lazied_mapper_filename, const std::string& lazied_prefix, const std::shared_ptr<Gateways::IDtoGateway>& gateway) : SceneGraph::SceneGraphStoreMapper(), m_spatialMap(mapper_filename, "", gateway), m_laziedMap(lazied_mapper_filename, lazied_prefix, gateway)
{
    m_hasConnected = false;
}

SceneGraphFileStoreMapper::~SceneGraphFileStoreMapper()
{
    assert(!m_hasConnected);
}

std::error_code SceneGraphFileStoreMapper::connect()
{
    if (m_hasConnected) return FileSystem::ErrorCode::ok;
    auto er = m_spatialMap.connect();
    if (er) return er;
    er = m_laziedMap.connect();
    m_hasConnected = true;
    return er;
}

std::error_code SceneGraphFileStoreMapper::disconnect()
{
    m_spatialMap.disconnect();
    m_laziedMap.disconnect();
    m_hasConnected = false;
    return FileSystem::ErrorCode::ok;
}

bool SceneGraphFileStoreMapper::hasCamera(const SceneGraph::SpatialId& id)
{
    assert(id.rtti().isDerived(SceneGraph::Camera::TYPE_RTTI));
    if (!m_hasConnected) connect();
    return m_spatialMap.has(id);
}

std::optional<Enigma::Engine::GenericDto> SceneGraphFileStoreMapper::queryCamera(const SceneGraph::SpatialId& id)
{
    assert(id.rtti().isDerived(SceneGraph::Camera::TYPE_RTTI));
    return m_spatialMap.query(id);
}

std::error_code SceneGraphFileStoreMapper::removeCamera(const SceneGraph::SpatialId& id)
{
    assert(id.rtti().isDerived(SceneGraph::Camera::TYPE_RTTI));
    return m_spatialMap.remove(id);
}

std::error_code SceneGraphFileStoreMapper::putCamera(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto)
{
    assert(id.rtti().isDerived(SceneGraph::Camera::TYPE_RTTI));
    return m_spatialMap.put(id, dto);
}

bool SceneGraphFileStoreMapper::hasSpatial(const SceneGraph::SpatialId& id)
{
    assert(id.rtti().isDerived(SceneGraph::Spatial::TYPE_RTTI));
    if (!m_hasConnected) connect();
    return m_spatialMap.has(id);
}

std::optional<Enigma::Engine::GenericDto> SceneGraphFileStoreMapper::querySpatial(const SceneGraph::SpatialId& id)
{
    assert(id.rtti().isDerived(SceneGraph::Spatial::TYPE_RTTI));
    return m_spatialMap.query(id);
}

std::error_code SceneGraphFileStoreMapper::removeSpatial(const SceneGraph::SpatialId& id)
{
    assert(id.rtti().isDerived(SceneGraph::Spatial::TYPE_RTTI));
    return m_spatialMap.remove(id);
}

std::error_code SceneGraphFileStoreMapper::putSpatial(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto)
{
    assert(id.rtti().isDerived(SceneGraph::Spatial::TYPE_RTTI));
    return m_spatialMap.put(id, dto);
}

bool SceneGraphFileStoreMapper::hasLaziedContent(const SceneGraph::SpatialId& id)
{
    return m_laziedMap.has(id);
}

std::optional<Enigma::Engine::GenericDto> SceneGraphFileStoreMapper::queryLaziedContent(const SceneGraph::SpatialId& id)
{
    return m_laziedMap.query(id);
}

std::error_code SceneGraphFileStoreMapper::removeLaziedContent(const SceneGraph::SpatialId& id)
{
    return m_laziedMap.remove(id);
}

std::error_code SceneGraphFileStoreMapper::putLaziedContent(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto)
{
    return m_laziedMap.put(id, dto);
}



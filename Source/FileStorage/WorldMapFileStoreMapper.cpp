#include "WorldMapFileStoreMapper.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileSystemErrors.h"
#include "Frameworks/TokenVector.h"
#include <cassert>

using namespace Enigma::FileStorage;
using namespace Enigma::WorldMap;

WorldMapFileStoreMapper::WorldMapFileStoreMapper(const std::string& world_mapper_filename, const std::string& quad_root_mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway)
    : m_gateway(gateway), m_worldMapperFilename(world_mapper_filename), m_quadRootMapperFilename(quad_root_mapper_filename)
{
    m_hasConnected = false;
}

WorldMapFileStoreMapper::~WorldMapFileStoreMapper()
{
    assert(!m_hasConnected);
    assert(m_worldFilenameMap.empty());
    assert(m_quadRootFilenameMap.empty());
}

std::error_code WorldMapFileStoreMapper::connect()
{
    if (m_hasConnected) return FileSystem::ErrorCode::ok;
    auto er = connectWorldMapperFile();
    if (!er) return er;
    er = connectQuadRootMapperFile();
    if (!er) return er;
    m_hasConnected = true;
    return FileSystem::ErrorCode::ok;
}

std::error_code WorldMapFileStoreMapper::disconnect()
{
    std::lock_guard locker{ m_worldMapLock };
    m_worldFilenameMap.clear();
    std::lock_guard locker2{ m_quadTreeRootLock };
    m_quadRootFilenameMap.clear();
    m_hasConnected = false;
    return FileSystem::ErrorCode::ok;
}

bool WorldMapFileStoreMapper::hasQuadTreeRoot(const QuadTreeRootId& id)
{
    if (!m_hasConnected) connect();
    std::lock_guard locker{ m_quadTreeRootLock };
    return m_quadRootFilenameMap.find(id) != m_quadRootFilenameMap.end();
}

std::optional<Enigma::Engine::GenericDto> WorldMapFileStoreMapper::queryQuadTreeRoot(const WorldMap::QuadTreeRootId& id)
{
    if (!m_hasConnected) connect();
    std::lock_guard locker{ m_quadTreeRootLock };
    auto it = m_quadRootFilenameMap.find(id);
    if (it == m_quadRootFilenameMap.end()) return std::nullopt;
    return deserializeDataTransferObjects(it->second);
}

std::error_code WorldMapFileStoreMapper::removeQuadTreeRoot(const WorldMap::QuadTreeRootId& id)
{
    if (!m_hasConnected) connect();
    std::lock_guard locker{ m_quadTreeRootLock };
    auto it = m_quadRootFilenameMap.find(id);
    if (it == m_quadRootFilenameMap.end()) return FileSystem::ErrorCode::ok;
    m_quadRootFilenameMap.erase(it);
    auto er = serializeQuadRootMapperFile();
    if (er) return er;
    return FileSystem::ErrorCode::ok;
}

std::error_code WorldMapFileStoreMapper::putQuadTreeRoot(const WorldMap::QuadTreeRootId& id, const Engine::GenericDto& dto)
{
    auto filename = extractQuadRootFilename(id, dto.getRtti());
    std::lock_guard locker{ m_quadTreeRootLock };
    m_quadRootFilenameMap.insert_or_assign(id, filename);
    auto er = serializeQuadRootMapperFile();
    if (er) return er;
    er = serializeDataTransferObjects(filename, dto);
    if (er) return er;
    return FileSystem::ErrorCode::ok;
}

bool WorldMapFileStoreMapper::hasWorldMap(const WorldMap::WorldMapId& id)
{
    if (!m_hasConnected) connect();
    std::lock_guard locker{ m_worldMapLock };
    return m_worldFilenameMap.find(id) != m_worldFilenameMap.end();
}

std::optional<Enigma::Engine::GenericDto> WorldMapFileStoreMapper::queryWorldMap(const WorldMap::WorldMapId& id)
{
    if (!m_hasConnected) connect();
    std::lock_guard locker{ m_worldMapLock };
    auto it = m_worldFilenameMap.find(id);
    if (it == m_worldFilenameMap.end()) return std::nullopt;
    return deserializeDataTransferObjects(it->second);
}

std::error_code WorldMapFileStoreMapper::removeWorldMap(const WorldMap::WorldMapId& id)
{
    if (!m_hasConnected) connect();
    std::lock_guard locker{ m_worldMapLock };
    auto it = m_worldFilenameMap.find(id);
    if (it == m_worldFilenameMap.end()) return FileSystem::ErrorCode::ok;
    m_worldFilenameMap.erase(it);
    auto er = serializeWorldMapperFile();
    if (er) return er;
    return FileSystem::ErrorCode::ok;
}

std::error_code WorldMapFileStoreMapper::putWorldMap(const WorldMap::WorldMapId& id, const Engine::GenericDto& dto)
{
    auto filename = extractWorldFilename(id, dto.getRtti());
    std::lock_guard locker{ m_worldMapLock };
    m_worldFilenameMap.insert_or_assign(id, filename);
    auto er = serializeWorldMapperFile();
    if (er) return er;
    er = serializeDataTransferObjects(filename, dto);
    if (er) return er;
    return FileSystem::ErrorCode::ok;
}

std::error_code WorldMapFileStoreMapper::connectWorldMapperFile()
{
    std::lock_guard locker{ m_worldMapLock };
    m_worldFilenameMap.clear();
    FileSystem::IFilePtr world_mapper_file = FileSystem::FileSystem::instance()->openFile(m_worldMapperFilename, FileSystem::read | FileSystem::binary);
    if (!world_mapper_file) return FileSystem::ErrorCode::ok;
    auto file_size = world_mapper_file->size();
    if (file_size == 0) return FileSystem::ErrorCode::ok;
    auto content = world_mapper_file->read(0, file_size);
    FileSystem::FileSystem::instance()->closeFile(world_mapper_file);
    if (!content) return FileSystem::ErrorCode::readFail;
    auto er = deserializeWorldMapperFile(std::string(content->begin(), content->end()));
    return er;
}

std::error_code WorldMapFileStoreMapper::connectQuadRootMapperFile()
{
    std::lock_guard locker{ m_quadTreeRootLock };
    m_quadRootFilenameMap.clear();
    FileSystem::IFilePtr quad_root_mapper_file = FileSystem::FileSystem::instance()->openFile(m_quadRootMapperFilename, FileSystem::read | FileSystem::binary);
    if (!quad_root_mapper_file) return FileSystem::ErrorCode::ok;
    auto file_size = quad_root_mapper_file->size();
    if (file_size == 0) return FileSystem::ErrorCode::ok;
    auto content = quad_root_mapper_file->read(0, file_size);
    FileSystem::FileSystem::instance()->closeFile(quad_root_mapper_file);
    if (!content) return FileSystem::ErrorCode::readFail;
    auto er = deserializeQuadRootMapperFile(std::string(content->begin(), content->end()));
    return er;
}

std::error_code WorldMapFileStoreMapper::deserializeWorldMapperFile(const std::string& content)
{
    if (content.empty()) return FileSystem::ErrorCode::ok;
    std::lock_guard locker{ m_worldMapLock };
    auto lines = split_token(content, "\n\r");
    for (auto& line : lines)
    {
        auto tokens = split_token(line, ",");
        if (tokens.size() != 2) continue;
        WorldMapId id{ tokens[0] };
        m_worldFilenameMap.insert_or_assign(id, tokens[1]);
    }
    return FileSystem::ErrorCode::ok;
}

std::error_code WorldMapFileStoreMapper::deserializeQuadRootMapperFile(const std::string& content)
{
    if (content.empty()) return FileSystem::ErrorCode::ok;
    std::lock_guard locker{ m_quadTreeRootLock };
    auto lines = split_token(content, "\n\r");
    for (auto& line : lines)
    {
        auto tokens = split_token(line, ",");
        if (tokens.size() != 2) continue;
        QuadTreeRootId id{ tokens[0] };
        m_quadRootFilenameMap.insert_or_assign(id, tokens[1]);
    }
    return FileSystem::ErrorCode::ok;
}

std::error_code WorldMapFileStoreMapper::serializeWorldMapperFile()
{
    std::lock_guard locker{ m_worldMapLock };
    std::string mapper_file_content;
    for (auto& rec : m_worldFilenameMap)
    {
        mapper_file_content += rec.first.name() + "," + rec.second + "\n";
    }
    auto mapper_file = FileSystem::FileSystem::instance()->openFile(m_worldMapperFilename, FileSystem::write | FileSystem::binary);
    if (!mapper_file) return FileSystem::ErrorCode::fileOpenError;
    auto write_size = mapper_file->write(0, { mapper_file_content.begin(), mapper_file_content.end() });
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (write_size != mapper_file_content.size()) return FileSystem::ErrorCode::writeFail;
    return FileSystem::ErrorCode::ok;
}

std::error_code WorldMapFileStoreMapper::serializeQuadRootMapperFile()
{
    std::lock_guard locker{ m_quadTreeRootLock };
    std::string mapper_file_content;
    for (auto& rec : m_quadRootFilenameMap)
    {
        mapper_file_content += rec.first.name() + "," + rec.second + "\n";
    }
    auto mapper_file = FileSystem::FileSystem::instance()->openFile(m_quadRootMapperFilename, FileSystem::write | FileSystem::binary);
    if (!mapper_file) return FileSystem::ErrorCode::fileOpenError;
    auto write_size = mapper_file->write(0, { mapper_file_content.begin(), mapper_file_content.end() });
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (write_size != mapper_file_content.size()) return FileSystem::ErrorCode::writeFail;
    return FileSystem::ErrorCode::ok;
}

Enigma::Engine::GenericDto WorldMapFileStoreMapper::deserializeDataTransferObjects(const std::string& filename)
{
    assert(m_gateway);
    auto dto_file = FileSystem::FileSystem::instance()->openFile(filename, FileSystem::read | FileSystem::binary);
    auto file_size = dto_file->size();
    auto content = dto_file->read(0, file_size);
    assert(content.has_value());
    FileSystem::FileSystem::instance()->closeFile(dto_file);
    auto dtos = m_gateway->deserialize({ std::string(content->begin(), content->end()) });
    assert(!dtos.empty());
    return dtos[0];
}

std::error_code WorldMapFileStoreMapper::serializeDataTransferObjects(const std::string& filename, const Engine::GenericDto& dto)
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

std::string WorldMapFileStoreMapper::extractWorldFilename(const WorldMap::WorldMapId& id, const Engine::FactoryDesc& factory_desc)
{
    if (!factory_desc.deferredFilename().empty()) return factory_desc.deferredFilename();
    if (!factory_desc.resourceFilename().empty()) return factory_desc.resourceFilename();
    return id.name() + ".json";
}

std::string WorldMapFileStoreMapper::extractQuadRootFilename(const WorldMap::QuadTreeRootId& id, const Engine::FactoryDesc& factory_desc)
{
    if (!factory_desc.deferredFilename().empty()) return factory_desc.deferredFilename();
    if (!factory_desc.resourceFilename().empty()) return factory_desc.resourceFilename();
    return id.name() + ".json";
}

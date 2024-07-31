#include "TextureFileStoreMapper.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileSystemErrors.h"
#include "Frameworks/TokenVector.h"
#include "GameEngine/EngineErrors.h"
#include <cassert>

using namespace Enigma::FileStorage;

TextureFileStoreMapper::TextureFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway) : Engine::TextureStoreMapper()
{
    m_mapper_filename = mapper_filename;
    m_gateway = gateway;
    m_has_connected = false;
}

TextureFileStoreMapper::~TextureFileStoreMapper()
{
    assert(!m_has_connected);
    assert(m_filename_map.empty());
}

std::error_code TextureFileStoreMapper::connect()
{
    if (m_has_connected) return FileSystem::ErrorCode::ok;
    std::lock_guard locker{ m_fileMapLock };
    m_filename_map.clear();
    FileSystem::IFilePtr mapper_file = FileSystem::FileSystem::instance()->openFile(m_mapper_filename, FileSystem::read | FileSystem::binary);
    if (!mapper_file)
    {
        m_has_connected = true;
        return FileSystem::ErrorCode::ok;
    }
    auto file_size = mapper_file->size();
    if (file_size == 0)
    {
        m_has_connected = true;
        return FileSystem::ErrorCode::ok;
    }
    auto content = mapper_file->read(0, file_size);
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (!content) return FileSystem::ErrorCode::readFail;
    deserializeMapperFile(std::string(content->begin(), content->end()));
    m_has_connected = true;
    return FileSystem::ErrorCode::ok;
}

std::error_code TextureFileStoreMapper::disconnect()
{
    std::lock_guard locker{ m_fileMapLock };
    m_filename_map.clear();
    m_has_connected = false;
    return FileSystem::ErrorCode::ok;
}

bool TextureFileStoreMapper::hasTexture(const Engine::TextureId& id)
{
    if (!m_has_connected) connect();
    std::lock_guard locker{ m_fileMapLock };
    return m_filename_map.find(id) != m_filename_map.end();
}

std::optional<Enigma::Engine::GenericDto> TextureFileStoreMapper::queryTexture(const Engine::TextureId& id)
{
    std::lock_guard locker{ m_fileMapLock };
    auto it = m_filename_map.find(id);
    if (it == m_filename_map.end()) return std::nullopt;
    return deserializeDataTransferObject(it->second);
}

std::error_code TextureFileStoreMapper::putTexture(const Engine::TextureId& id, const Engine::GenericDto& dto)
{
    auto filename = extractFilename(id, dto.getRtti());
    std::lock_guard locker{ m_fileMapLock };
    m_filename_map.insert_or_assign(id, filename);
    auto er = serializeMapperFile();
    if (er) return er;
    er = serializeDataTransferObject(filename, dto);
    if (er) return er;
    return Engine::ErrorCode::ok;
}

std::error_code TextureFileStoreMapper::removeTexture(const Engine::TextureId& id)
{
    std::lock_guard locker{ m_fileMapLock };
    m_filename_map.erase(id);
    auto er = serializeMapperFile();
    if (er) return er;
    return Engine::ErrorCode::ok;
}

std::error_code TextureFileStoreMapper::serializeMapperFile()
{
    std::lock_guard locker{ m_fileMapLock };
    std::string mapper_file_content = "";
    for (auto& rec : m_filename_map)
    {
        mapper_file_content += rec.first.name() + "," + rec.second + "\n";
    }
    auto mapper_file = FileSystem::FileSystem::instance()->openFile(m_mapper_filename, FileSystem::write | FileSystem::binary);
    if (!mapper_file) return FileSystem::ErrorCode::fileOpenError;
    auto write_size = mapper_file->write(0, { mapper_file_content.begin(), mapper_file_content.end() });
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (write_size != mapper_file_content.size()) return FileSystem::ErrorCode::writeFail;
    return FileSystem::ErrorCode::ok;
}

void TextureFileStoreMapper::deserializeMapperFile(const std::string& mapper_file_content)
{
    if (mapper_file_content.empty()) return;
    std::lock_guard locker{ m_fileMapLock };
    auto lines = split_token(mapper_file_content, "\n\r");
    for (auto& line : lines)
    {
        auto tokens = split_token(line, ",");
        if (tokens.size() != 2) continue;
        Engine::TextureId id{ tokens[0] };
        m_filename_map.insert_or_assign(id, tokens[1]);
    }
}

std::error_code TextureFileStoreMapper::serializeDataTransferObject(const std::string& filename, const Engine::GenericDto& dto)
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

Enigma::Engine::GenericDto TextureFileStoreMapper::deserializeDataTransferObject(const std::string& filename)
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

std::string TextureFileStoreMapper::extractFilename(const Engine::TextureId& id, const Engine::FactoryDesc& factory_desc)
{
    if (!factory_desc.GetDeferredFilename().empty()) return factory_desc.GetDeferredFilename();
    if (!factory_desc.GetResourceFilename().empty()) return factory_desc.GetResourceFilename();
    return id.name() + ".json";
}

﻿#include "AnimatorFileStoreMapper.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/FileSystemErrors.h"
#include "Animators/AnimatorErrors.h"
#include "Frameworks/TokenVector.h"
#include <cassert>

using namespace Enigma::FileStorage;

AnimatorFileStoreMapper::AnimatorFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway)
{
    m_mapper_filename = mapper_filename;
    m_gateway = gateway;
    m_has_connected = false;
    m_sequence_number = 0;
}

AnimatorFileStoreMapper::~AnimatorFileStoreMapper()
{
    assert(!m_has_connected);
    assert(m_filename_map.empty());
}

std::error_code AnimatorFileStoreMapper::connect()
{
    if (m_has_connected) return FileSystem::ErrorCode::ok;
    std::lock_guard locker{ m_file_map_lock };
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

std::error_code AnimatorFileStoreMapper::disconnect()
{
    std::lock_guard locker{ m_file_map_lock };
    m_filename_map.clear();
    m_has_connected = false;
    return FileSystem::ErrorCode::ok;
}

bool AnimatorFileStoreMapper::hasAnimator(const Animators::AnimatorId& id)
{
    if (!m_has_connected) connect();
    std::lock_guard locker{ m_file_map_lock };
    return m_filename_map.find(id) != m_filename_map.end();
}

std::optional<Enigma::Engine::GenericDto> AnimatorFileStoreMapper::queryAnimator(const Animators::AnimatorId& id)
{
    std::lock_guard locker{ m_file_map_lock };
    auto it = m_filename_map.find(id);
    if (it == m_filename_map.end()) return std::nullopt;
    return deserializeDataTransferObject(it->second);
}

std::error_code AnimatorFileStoreMapper::removeAnimator(const Animators::AnimatorId& id)
{
    std::lock_guard locker{ m_file_map_lock };
    m_filename_map.erase(id);
    auto er = serializeMapperFile();
    if (er) return er;
    return Animators::ErrorCode::ok;
}

std::error_code AnimatorFileStoreMapper::putAnimator(const Animators::AnimatorId& id, const Engine::GenericDto& dto)
{
    auto filename = extractFilename(id, dto.getRtti());
    std::lock_guard locker{ m_file_map_lock };
    m_filename_map.insert_or_assign(id, filename);
    auto er = serializeMapperFile();
    if (er) return er;
    er = serializeDataTransferObject(filename, dto);
    if (er) return er;
    return Animators::ErrorCode::ok;
}

std::uint64_t AnimatorFileStoreMapper::nextSequenceNumber()
{
    std::lock_guard locker{ m_file_map_lock };
    return ++m_sequence_number;
}

void AnimatorFileStoreMapper::deserializeMapperFile(const std::string& mapper_file_content)
{
    if (mapper_file_content.empty()) return;
    std::lock_guard locker{ m_file_map_lock };
    auto lines = split_token(mapper_file_content, "\n\r");
    m_sequence_number = std::stoul(lines[0]);
    for (auto& line : lines)
    {
        auto tokens = split_token(line, ",");
        if (tokens.size() != 4) continue; // then, skip first line
        Animators::AnimatorId id{ tokens[0], std::stoull(tokens[1]), Frameworks::Rtti::fromName(tokens[2]) };
        m_filename_map.insert_or_assign(id, tokens[3]);
    }
}

std::error_code AnimatorFileStoreMapper::serializeMapperFile()
{
    std::lock_guard locker{ m_file_map_lock };
    std::string mapper_file_content = std::to_string(m_sequence_number) + "\n";
    for (auto& rec : m_filename_map)
    {
        mapper_file_content += rec.first.name() + "," + std::to_string(rec.first.sequence()) + "," + rec.first.rtti().getName() + "," + rec.second + "\n";
    }
    auto mapper_file = FileSystem::FileSystem::instance()->openFile(m_mapper_filename, FileSystem::write | FileSystem::binary);
    if (!mapper_file) return FileSystem::ErrorCode::fileOpenError;
    auto write_size = mapper_file->write(0, { mapper_file_content.begin(), mapper_file_content.end() });
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (write_size != mapper_file_content.size()) return FileSystem::ErrorCode::writeFail;
    return FileSystem::ErrorCode::ok;
}

std::string AnimatorFileStoreMapper::extractFilename(const Animators::AnimatorId& id, const Engine::FactoryDesc& factory_desc)
{
    if (!factory_desc.deferredFilename().empty()) return factory_desc.deferredFilename();
    if (!factory_desc.resourceFilename().empty()) return factory_desc.resourceFilename();
    return id.name() + ".json";
}

Enigma::Engine::GenericDto AnimatorFileStoreMapper::deserializeDataTransferObject(const std::string& filename)
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

std::error_code AnimatorFileStoreMapper::serializeDataTransferObject(const std::string& filename, const Engine::GenericDto& dto)
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

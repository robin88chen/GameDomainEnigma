#include "EffectMaterialSourceFileStoreMapper.h"
#include "FileSystem/FileSystemErrors.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "Frameworks/TokenVector.h"

using namespace Enigma::FileStorage;
using namespace Enigma::Engine;

EffectMaterialSourceFileStoreMapper::EffectMaterialSourceFileStoreMapper(const std::string& mapper_filename) : EffectMaterialSourceStoreMapper(), m_mapper_filename(mapper_filename)
{
    m_has_connected = false;
}

EffectMaterialSourceFileStoreMapper::~EffectMaterialSourceFileStoreMapper()
{
    assert(!m_has_connected);
}

std::error_code EffectMaterialSourceFileStoreMapper::connect()
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
    auto content = mapper_file->read(0, file_size);
    FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (!content) return FileSystem::ErrorCode::readFail;
    deserializeMapperFile(std::string(content->begin(), content->end()));
    m_has_connected = true;
    return FileSystem::ErrorCode::ok;
}

std::error_code EffectMaterialSourceFileStoreMapper::disconnect()
{
    std::lock_guard locker{ m_fileMapLock };
    m_filename_map.clear();
    m_has_connected = false;
    return FileSystem::ErrorCode::ok;
}

bool EffectMaterialSourceFileStoreMapper::hasEffectMaterial(const EffectMaterialId& id)
{
    if (!m_has_connected) connect();
    std::lock_guard locker{ m_fileMapLock };
    return m_filename_map.find(id) != m_filename_map.end();
}

std::optional<EffectCompilingProfile> EffectMaterialSourceFileStoreMapper::queryEffectMaterial(const EffectMaterialId& id)
{
    std::lock_guard locker{ m_fileMapLock };
    auto it = m_filename_map.find(id);
    if (it == m_filename_map.end()) return std::nullopt;
    return deserializeEffectCompilingProfile(it->second);
}

void EffectMaterialSourceFileStoreMapper::deserializeMapperFile(const std::string& mapper_file_content)
{
    if (mapper_file_content.empty()) return;
    std::lock_guard locker{ m_fileMapLock };
    auto lines = split_token(mapper_file_content, "\n\r");
    for (auto& line : lines)
    {
        auto tokens = split_token(line, ",");
        if (tokens.size() != 2) continue;
        EffectMaterialId id{ tokens[0] };
        m_filename_map.insert_or_assign(id, tokens[1]);
    }
}

std::optional<EffectCompilingProfile> EffectMaterialSourceFileStoreMapper::deserializeEffectCompilingProfile(const std::string& filename)
{
    m_gateway.cleanup();
    auto efx_file = FileSystem::FileSystem::instance()->openFile(filename, FileSystem::read | FileSystem::binary);
    auto file_size = efx_file->size();
    auto content = efx_file->read(0, file_size);
    assert(content.has_value());
    FileSystem::FileSystem::instance()->closeFile(efx_file);
    return m_gateway.Deserialize(std::string(content.value().begin(), content.value().end()));
}

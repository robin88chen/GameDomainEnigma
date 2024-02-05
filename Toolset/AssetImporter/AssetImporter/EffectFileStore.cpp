#include "EffectFileStore.h"
#include "FileSystem/FileSystem.h"

void AssetImporter::EffectFileStore::appendEffectMaterial(const Enigma::Engine::EffectMaterialId& id, const std::string& filename)
{
    std::lock_guard locker{ m_fileMapLock };
    m_filename_map[id] = filename;
}

void AssetImporter::EffectFileStore::serializeMapperFile()
{
    std::lock_guard locker{ m_fileMapLock };
    std::string mapper_file_content = "";
    for (auto& rec : m_filename_map)
    {
        mapper_file_content += rec.first.name() + "," + rec.second + "\n";
    }
    auto mapper_file = Enigma::FileSystem::FileSystem::instance()->openFile(m_mapper_filename, Enigma::FileSystem::write | Enigma::FileSystem::binary);
    if (!mapper_file) return;
    auto write_size = mapper_file->write(0, { mapper_file_content.begin(), mapper_file_content.end() });
    Enigma::FileSystem::FileSystem::FileSystem::instance()->closeFile(mapper_file);
    if (write_size != mapper_file_content.size()) return;
}

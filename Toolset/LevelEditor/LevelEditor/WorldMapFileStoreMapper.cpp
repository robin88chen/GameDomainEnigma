#include "WorldMapFileStoreMapper.h"

using namespace LevelEditor;

WorldMapFileStoreMapper::WorldMapFileStoreMapper(const std::string& world_mapper_filename, const std::string& quad_root_mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway) : Enigma::FileStorage::WorldMapFileStoreMapper(world_mapper_filename, quad_root_mapper_filename, gateway)
{
}

WorldMapFileStoreMapper::~WorldMapFileStoreMapper()
{
}

bool WorldMapFileStoreMapper::isWorldNameDuplicated(const std::string& world_name)
{
    std::lock_guard lock{ m_worldMapLock };
    for (const auto& pair : m_worldFilenameMap)
    {
        if (pair.first.name() == world_name)
        {
            return true;
        }
    }
    return false;
}

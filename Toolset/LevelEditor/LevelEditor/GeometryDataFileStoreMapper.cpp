#include "GeometryDataFileStoreMapper.h"

using namespace LevelEditor;

GeometryDataFileStoreMapper::GeometryDataFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway) : Enigma::FileStorage::GeometryDataFileStoreMapper(mapper_filename, gateway)
{
}

GeometryDataFileStoreMapper::~GeometryDataFileStoreMapper()
{
}

bool GeometryDataFileStoreMapper::isGeometryNameDuplicated(const std::string& geometry_name)
{
    std::lock_guard lock{ m_fileMapLock };
    for (const auto& pair : m_filename_map)
    {
        if (pair.first.name() == geometry_name)
        {
            return true;
        }
    }
    return false;
}


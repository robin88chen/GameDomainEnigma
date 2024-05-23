#include "SceneGraphFileStoreMapper.h"
#include <algorithm>

using namespace LevelEditor;

SceneGraphFileStoreMapper::SceneGraphFileStoreMapper(const std::string& mapper_filename, const std::string& lazied_mapper_filename, const std::string& lazied_prefix, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway) : Enigma::FileStorage::SceneGraphFileStoreMapper(mapper_filename, lazied_mapper_filename, lazied_prefix, gateway)
{
    subscribeHandlers();
}

SceneGraphFileStoreMapper::~SceneGraphFileStoreMapper()
{
    unsubscribeHandlers();
}

void SceneGraphFileStoreMapper::subscribeHandlers()
{

}

void SceneGraphFileStoreMapper::unsubscribeHandlers()
{

}

std::optional<Enigma::SceneGraph::SpatialId> SceneGraphFileStoreMapper::spatialId(const std::string& spatial_name) const
{
    for (const auto& [id, name] : m_spatialMap.map())
    {
        if (id.name() == spatial_name)
        {
            return id;
        }
    }
    return std::nullopt;
}

bool SceneGraphFileStoreMapper::isSpatialNameDuplicated(const std::string& spatial_name) const
{
    return std::any_of(m_spatialMap.map().begin(), m_spatialMap.map().end(),
        [&](const auto& pair) { return pair.first.name() == spatial_name; });
}

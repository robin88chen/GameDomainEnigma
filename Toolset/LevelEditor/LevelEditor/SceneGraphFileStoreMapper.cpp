#include "SceneGraphFileStoreMapper.h"
#include "LevelEditorQueries.h"
#include "Frameworks/QueryDispatcher.h"
#include "SceneGraph/Node.h"
#include "Terrain/TerrainPawn.h"
#include <algorithm>

using namespace LevelEditor;

SceneGraphFileStoreMapper::SceneGraphFileStoreMapper(const std::string& mapper_filename, const std::string& lazied_mapper_filename, const std::string& lazied_prefix, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway) : Enigma::FileStorage::SceneGraphFileStoreMapper(mapper_filename, lazied_mapper_filename, lazied_prefix, gateway)
{
}

SceneGraphFileStoreMapper::~SceneGraphFileStoreMapper()
{
}

void SceneGraphFileStoreMapper::subscribeHandlers()
{
    m_isSpatialNameDuplicated = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { isSpatialNameDuplicated(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(IsSpatialNameDuplicated), m_isSpatialNameDuplicated);
    m_queryNodeIds = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { queryNodeIds(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(QueryNodeIds), m_queryNodeIds);
    m_queryTerrainIds = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { queryTerrainIds(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(QueryTerrainIds), m_queryTerrainIds);
}

void SceneGraphFileStoreMapper::unsubscribeHandlers()
{
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(IsSpatialNameDuplicated), m_isSpatialNameDuplicated);
    m_isSpatialNameDuplicated = nullptr;
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(QueryNodeIds), m_queryNodeIds);
    m_queryNodeIds = nullptr;
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(QueryTerrainIds), m_queryTerrainIds);
    m_queryTerrainIds = nullptr;
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

std::vector<Enigma::SceneGraph::SpatialId> SceneGraphFileStoreMapper::nodeIds() const
{
    std::vector<Enigma::SceneGraph::SpatialId> ids;
    for (const auto& [id, name] : m_spatialMap.map())
    {
        if (id.rtti().isDerived(Enigma::SceneGraph::Node::TYPE_RTTI))
        {
            ids.push_back(id);
        }
    }
    return ids;
}

std::vector<Enigma::SceneGraph::SpatialId> SceneGraphFileStoreMapper::terrainIds() const
{
    std::vector<Enigma::SceneGraph::SpatialId> ids;
    for (const auto& [id, name] : m_spatialMap.map())
    {
        if (id.rtti().isDerived(Enigma::Terrain::TerrainPawn::TYPE_RTTI))
        {
            ids.push_back(id);
        }
    }
    return ids;
}

bool SceneGraphFileStoreMapper::isSpatialNameDuplicated(const std::string& spatial_name) const
{
    return std::any_of(m_spatialMap.map().begin(), m_spatialMap.map().end(),
        [&](const auto& pair) { return pair.first.name() == spatial_name; });
}

void SceneGraphFileStoreMapper::isSpatialNameDuplicated(const Enigma::Frameworks::IQueryPtr& q)
{
    if (!q) return;
    auto query = std::dynamic_pointer_cast<IsSpatialNameDuplicated>(q);
    query->setResult(isSpatialNameDuplicated(query->spatialName()));
}

void SceneGraphFileStoreMapper::queryNodeIds(const Enigma::Frameworks::IQueryPtr& q)
{
    if (!q) return;
    auto query = std::dynamic_pointer_cast<QueryNodeIds>(q);
    query->setResult(nodeIds());
}

void SceneGraphFileStoreMapper::queryTerrainIds(const Enigma::Frameworks::IQueryPtr& q)
{
    if (!q) return;
    auto query = std::dynamic_pointer_cast<QueryTerrainIds>(q);
    query->setResult(terrainIds());
}

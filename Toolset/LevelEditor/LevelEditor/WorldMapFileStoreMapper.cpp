﻿#include "WorldMapFileStoreMapper.h"
#include "LevelEditorQueries.h"
#include "Frameworks/QueryDispatcher.h"

using namespace LevelEditor;

WorldMapFileStoreMapper::WorldMapFileStoreMapper(const std::string& world_mapper_filename, const std::string& quad_root_mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway) : Enigma::FileStorage::WorldMapFileStoreMapper(world_mapper_filename, quad_root_mapper_filename, gateway)
{
}

WorldMapFileStoreMapper::~WorldMapFileStoreMapper()
{
}

void WorldMapFileStoreMapper::subscribeHandlers()
{
    m_isWorldNameDuplicated = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { isWorldNameDuplicated(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(IsWorldNameDuplicated), m_isWorldNameDuplicated);
    m_resolveWorldMapId = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { resolveWorldMapId(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(ResolveWorldId), m_resolveWorldMapId);
    m_queryWorldMapIds = std::make_shared<Enigma::Frameworks::QuerySubscriber>([=](const Enigma::Frameworks::IQueryPtr& q) { queryWorldMapIds(q); });
    Enigma::Frameworks::QueryDispatcher::subscribe(typeid(QueryWorldMapIds), m_queryWorldMapIds);
}

void WorldMapFileStoreMapper::unsubscribeHandlers()
{
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(IsWorldNameDuplicated), m_isWorldNameDuplicated);
    m_isWorldNameDuplicated = nullptr;
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(ResolveWorldId), m_resolveWorldMapId);
    m_resolveWorldMapId = nullptr;
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(QueryWorldMapIds), m_queryWorldMapIds);
    m_queryWorldMapIds = nullptr;
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

std::optional<Enigma::WorldMap::WorldMapId> WorldMapFileStoreMapper::worldMapId(const std::string& filename)
{
    std::lock_guard lock{ m_worldMapLock };
    for (const auto& pair : m_worldFilenameMap)
    {
        if (pair.second == filename)
        {
            return pair.first;
        }
    }
    return std::nullopt;
}

std::vector<Enigma::WorldMap::WorldMapId> WorldMapFileStoreMapper::worldMapIds()
{
    std::lock_guard lock{ m_worldMapLock };
    std::vector<Enigma::WorldMap::WorldMapId> world_map_ids;
    for (const auto& pair : m_worldFilenameMap)
    {
        world_map_ids.push_back(pair.first);
    }
    return world_map_ids;
}

void WorldMapFileStoreMapper::isWorldNameDuplicated(const Enigma::Frameworks::IQueryPtr& q)
{
    auto query = std::dynamic_pointer_cast<IsWorldNameDuplicated>(q);
    if (query)
    {
        query->setResult(isWorldNameDuplicated(query->worldName()));
    }
}

void WorldMapFileStoreMapper::resolveWorldMapId(const Enigma::Frameworks::IQueryPtr& q)
{
    auto query = std::dynamic_pointer_cast<ResolveWorldId>(q);
    if (query)
    {
        query->setResult(worldMapId(query->filename()));
    }
}

void WorldMapFileStoreMapper::queryWorldMapIds(const Enigma::Frameworks::IQueryPtr& q)
{
    auto query = std::dynamic_pointer_cast<QueryWorldMapIds>(q);
    if (query)
    {
        query->setResult(worldMapIds());
    }
}

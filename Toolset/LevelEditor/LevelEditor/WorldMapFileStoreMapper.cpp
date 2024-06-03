#include "WorldMapFileStoreMapper.h"
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
}

void WorldMapFileStoreMapper::unsubscribeHandlers()
{
    Enigma::Frameworks::QueryDispatcher::unsubscribe(typeid(IsWorldNameDuplicated), m_isWorldNameDuplicated);
    m_isWorldNameDuplicated = nullptr;
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

void WorldMapFileStoreMapper::isWorldNameDuplicated(const Enigma::Frameworks::IQueryPtr& q)
{
    auto query = std::dynamic_pointer_cast<IsWorldNameDuplicated>(q);
    if (query)
    {
        query->setResult(isWorldNameDuplicated(query->worldName()));
    }
}

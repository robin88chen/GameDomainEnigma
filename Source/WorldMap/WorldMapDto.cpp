#include "WorldMapDto.h"
#include "WorldMap.h"

using namespace Enigma::WorldMap;

static std::string TOKEN_ID = "ID";
static std::string TOKEN_OUT_REGION_ID = "OutRegionId";
static std::string TOKEN_QUAD_ROOT_IDS = "QuadRootIds";

WorldMapDto::WorldMapDto() : m_factoryDesc(WorldMap::TYPE_RTTI)
{
}

WorldMapDto::WorldMapDto(const Engine::GenericDto& dto) : m_factoryDesc(WorldMap::TYPE_RTTI)
{
    factoryDesc(dto.getRtti());
    if (auto v = dto.tryGetValue<std::string>(TOKEN_ID)) id(v.value());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_OUT_REGION_ID)) outRegionId(v.value());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_QUAD_ROOT_IDS))
    {
        std::vector<QuadTreeRootId> ids;
        for (const auto& id : v.value())
        {
            ids.emplace_back(id);
        }
        quadRootIds(ids);
    }
}

Enigma::Engine::GenericDto WorldMapDto::toGenericDto() const
{
    Engine::GenericDto dto;
    dto.addRtti(factoryDesc());
    dto.addOrUpdate(TOKEN_ID, id().name());
    if (outRegionId().has_value()) dto.addOrUpdate(TOKEN_OUT_REGION_ID, outRegionId().value().tokens());
    std::vector<std::string> ids;
    for (const auto& id : quadRootIds())
    {
        ids.emplace_back(id.name());
    }
    dto.addOrUpdate(TOKEN_QUAD_ROOT_IDS, ids);
    return dto;
}


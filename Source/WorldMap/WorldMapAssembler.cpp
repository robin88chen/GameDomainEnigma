#include "WorldMapAssembler.h"
#include "WorldMap.h"

using namespace Enigma::WorldMap;

static std::string TOKEN_ID = "ID";
static std::string TOKEN_OUT_REGION_ID = "OutRegionId";
static std::string TOKEN_QUAD_ROOT_IDS = "QuadRootIds";

WorldMapAssembler::WorldMapAssembler(const WorldMapId& id) : m_id(id), m_factoryDesc(Engine::FactoryDesc(WorldMap::TYPE_RTTI))
{
}

Enigma::Engine::GenericDto WorldMapAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.name());
    if (m_outRegionId.has_value()) dto.addOrUpdate(TOKEN_OUT_REGION_ID, m_outRegionId.value().tokens());
    std::vector<std::string> ids;
    for (const auto& id : m_quadRootIds)
    {
        ids.emplace_back(id.name());
    }
    dto.addOrUpdate(TOKEN_QUAD_ROOT_IDS, ids);
    return dto;
}

WorldMapDisassembler::WorldMapDisassembler() : m_factoryDesc(Engine::FactoryDesc(WorldMap::TYPE_RTTI))
{
}

void WorldMapDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_ID)) m_id = WorldMapId(v.value());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_OUT_REGION_ID)) m_outRegionId = SceneGraph::SpatialId(v.value());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_QUAD_ROOT_IDS))
    {
        for (const auto& id : v.value())
        {
            m_quadRootIds.emplace_back(id);
        }
    }
}

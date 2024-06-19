#include "WorldMapCommands.h"
#include "WorldMapDto.h"
#include "WorldMapQueries.h"
#include "WorldMapEvents.h"
#include "SceneGraph/PortalCommands.h"
#include "SceneGraph/SceneGraphAssemblers.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/PortalZoneNode.h"

using namespace Enigma::WorldMap;

void CreateEmptyWorldMap::execute()
{
    WorldMapDto world_map_dto;
    world_map_dto.id(m_id);
    world_map_dto.factoryDesc(m_factory_desc);
    if (m_outside_region_id.has_value())
    {
        world_map_dto.outRegionId(m_outside_region_id.value());
    }
    auto world = std::make_shared<RequestWorldMapConstitution>(m_id, world_map_dto.toGenericDto())->dispatch();
    if (world)
    {
        std::make_shared<WorldMapCreated>(m_id)->enqueue();
    }
}

void CreateWorldMapOutsideRegion::execute()
{
    SceneGraph::PortalZoneNodeAssembler region_assembler(m_id);
    region_assembler.factory(m_factory_desc);
    auto region = region_assembler.constituteDeHydrated();
    std::make_shared<SceneGraph::PutSpatial>(m_id, region)->execute();
    region->hydrate(region_assembler.toHydratedGenericDto());
    std::make_shared<SceneGraph::PutLaziedContent>(m_id, region)->execute();
    std::make_shared<SceneGraph::AttachPortalOutsideZone>(region)->enqueue();
}

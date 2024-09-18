#include "WorldMapCommands.h"
#include "WorldMapAssembler.h"
#include "WorldMapQueries.h"
#include "WorldMapEvents.h"
#include "SceneGraph/PortalCommands.h"
#include "SceneGraph/OutRegionNodeAssembler.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/OutRegionNode.h"

using namespace Enigma::WorldMap;

void CreateEmptyWorldMap::execute()
{
    WorldMapAssembler world_map_assembler(m_id);
    world_map_assembler.factoryDesc(m_factory_desc);
    if (m_outside_region_id.has_value())
    {
        world_map_assembler.outRegionId(m_outside_region_id.value());
    }
    auto world = std::make_shared<RequestWorldMapConstitution>(m_id, world_map_assembler.assemble())->dispatch();
    if (world)
    {
        std::make_shared<WorldMapCreated>(m_id, world)->enqueue();
    }
}

void CreateWorldMapOutsideRegion::execute()
{
    std::shared_ptr<SceneGraph::DehydratedOutRegionNodeAssembler> region_assembler = std::make_shared<SceneGraph::DehydratedOutRegionNodeAssembler>(m_id);
    region_assembler->factory(m_factory_desc);
    std::shared_ptr<SceneGraph::OutRegionNode> region = std::dynamic_pointer_cast<Enigma::SceneGraph::OutRegionNode>(std::make_shared<SceneGraph::RequestSpatialConstitution>(m_id, region_assembler->assemble())->dispatch());
    std::make_shared<SceneGraph::PutSpatial>(m_id, region)->execute();
    std::shared_ptr<SceneGraph::HydratedLazyNodeDisassembler> region_disassembler = region->disassemblerOfLaziedContent();
    region_disassembler->disassemble(region_assembler->assemble());
    region->hydrate(region_disassembler);
    std::make_shared<SceneGraph::PutLaziedContent>(m_id, region)->execute();
    std::make_shared<SceneGraph::AttachManagementOutsideRegion>(region)->enqueue();
}

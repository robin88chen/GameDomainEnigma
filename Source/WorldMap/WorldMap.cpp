#include "WorldMap.h"
#include "QuadTreeRoot.h"
#include "WorldMapAssembler.h"
#include "SceneGraph/SceneGraphCommands.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(WorldMap, WorldMap);

WorldMap::WorldMap(const WorldMapId& id) : m_factoryDesc(WorldMap::TYPE_RTTI)
{
    m_id = id;
}

WorldMap::WorldMap(const WorldMapId& id, const SceneGraph::SpatialId& out_region_id, const std::vector<QuadTreeRootId>& quad_roots) : m_factoryDesc(WorldMap::TYPE_RTTI)
{
    m_id = id;
    m_outRegionId = out_region_id;
    m_quadRootIds = quad_roots;
    if (!m_quadRootIds.empty()) m_quadRoots.resize(m_quadRootIds.size());
}

WorldMap::~WorldMap()
{
}

std::shared_ptr<WorldMapAssembler> WorldMap::assembler() const
{
    return std::make_shared<WorldMapAssembler>(m_id);
}

std::shared_ptr<WorldMapAssembler> WorldMap::assembledAssembler() const
{
    auto assembler = std::make_shared<WorldMapAssembler>(m_id);
    assemble(assembler);
    return assembler;
}

void WorldMap::assemble(const std::shared_ptr<WorldMapAssembler>& assembler) const
{
    assert(assembler);
    assembler->factoryDesc(m_factoryDesc);
    if (!m_outRegionId.empty()) assembler->outRegionId(m_outRegionId);
    assembler->quadRootIds(m_quadRootIds);
}

std::shared_ptr<WorldMapDisassembler> WorldMap::disassembler() const
{
    return std::make_shared<WorldMapDisassembler>();
}

void WorldMap::disassemble(const std::shared_ptr<WorldMapDisassembler>& disassembler)
{
    assert(disassembler);
    m_id = disassembler->id();
    m_factoryDesc = disassembler->factoryDesc();
    if (disassembler->outRegionId().has_value()) m_outRegionId = disassembler->outRegionId().value();
    m_quadRootIds = disassembler->quadRootIds();
    if (!m_quadRootIds.empty()) m_quadRoots.resize(m_quadRootIds.size());
}

std::shared_ptr<Enigma::SceneGraph::LazyNode> WorldMap::findFittingNode(const Engine::BoundingVolume& bv_in_world)
{
    if (m_quadRootIds.empty()) return nullptr;
    if (m_quadRootIds.size() != m_quadRoots.size()) return nullptr;
    for (size_t i = 0; i < m_quadRootIds.size(); ++i)
    {
        if ((m_quadRoots[i].expired()) || (m_quadRoots[i].lock()->id() != m_quadRootIds[i]))
        {
            m_quadRoots[i] = QuadTreeRoot::queryQuadTreeRoot(m_quadRootIds[i]);
        }
        if (m_quadRoots[i].expired()) continue;
        auto node_id = m_quadRoots[i].lock()->findFittingNode(bv_in_world);
        if (node_id) return std::dynamic_pointer_cast<SceneGraph::LazyNode>(SceneGraph::Node::queryNode(node_id.value()));
    }
    return nullptr;
}

void WorldMap::putOutRegion()
{
    if (m_outRegionId.empty()) return;
    std::shared_ptr<SceneGraph::Spatial> region = SceneGraph::Spatial::querySpatial(m_outRegionId);
    if (!region) return;
    std::make_shared<SceneGraph::PutSpatial>(m_outRegionId, region)->execute();
    std::shared_ptr<SceneGraph::LazyNode> lazy_region = std::dynamic_pointer_cast<SceneGraph::LazyNode>(region);
    if (!lazy_region) return;
    std::make_shared<SceneGraph::PutLaziedContent>(m_outRegionId, lazy_region)->execute();
}

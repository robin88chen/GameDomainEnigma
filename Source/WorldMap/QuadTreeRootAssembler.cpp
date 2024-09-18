#include "QuadTreeRootAssembler.h"
#include "QuadTreeRoot.h"

using namespace Enigma::WorldMap;

static std::string TOKEN_ID = "ID";
static std::string TOKEN_ROOT_ID = "RootID";

QuadTreeRootAssembler::QuadTreeRootAssembler(const QuadTreeRootId& id) : m_id(id), m_factoryDesc(Engine::FactoryDesc(QuadTreeRoot::TYPE_RTTI))
{
}

Enigma::Engine::GenericDto QuadTreeRootAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.name());
    dto.addOrUpdate(TOKEN_ROOT_ID, m_rootNodeId.tokens());
    return dto;
}

QuadTreeRootDisassembler::QuadTreeRootDisassembler() : m_factoryDesc(Engine::FactoryDesc(QuadTreeRoot::TYPE_RTTI))
{
}

void QuadTreeRootDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_ID)) m_id = QuadTreeRootId(v.value());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ROOT_ID)) m_rootNodeId = SceneGraph::SpatialId(v.value());
}

#include "QuadTreeRoot.h"
#include "QuadTreeRootDto.h"
#include <cassert>

using namespace Enigma::WorldMap;

QuadTreeRoot::QuadTreeRoot(const QuadTreeRootId& id) : m_id(id)
{

}

QuadTreeRoot::QuadTreeRoot(const QuadTreeRootId& id, const Engine::GenericDto& o) : m_id(id)
{
    QuadTreeRootDto dto(o);
    assert(dto.id() == m_id);
    m_rootNodeId = dto.rootNodeId();
}


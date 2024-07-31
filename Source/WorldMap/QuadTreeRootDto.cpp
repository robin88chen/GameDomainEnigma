#include "QuadTreeRootDto.h"
#include "QuadTreeRoot.h"

using namespace Enigma::WorldMap;

static std::string TOKEN_ID = "ID";
static std::string TOKEN_ROOT_ID = "RootID";

QuadTreeRootDto::QuadTreeRootDto() : m_factoryDesc(QuadTreeRoot::TYPE_RTTI)
{
}

QuadTreeRootDto::QuadTreeRootDto(const Engine::GenericDto& dto) : m_factoryDesc(QuadTreeRoot::TYPE_RTTI)
{
    m_factoryDesc = dto.getRtti();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_ID)) id(v.value());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ROOT_ID)) rootNodeId(v.value());
}

Enigma::Engine::GenericDto QuadTreeRootDto::toGenericDto() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, id().name());
    dto.addOrUpdate(TOKEN_ROOT_ID, rootNodeId().tokens());
    return dto;
}


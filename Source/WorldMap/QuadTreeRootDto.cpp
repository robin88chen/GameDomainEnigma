#include "QuadTreeRootDto.h"
#include "SceneQuadTreeRoot.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Engine;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_ROOT = "Root";

QuadTreeRootDto::QuadTreeRootDto() : m_factoryDesc(SceneQuadTreeRoot::TYPE_RTTI.getName())
{
}

QuadTreeRootDto QuadTreeRootDto::fromGenericDto(const Engine::GenericDto& dto)
{
    QuadTreeRootDto quad_dto;
    quad_dto.factoryDesc() = dto.getRtti();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_NAME)) quad_dto.name() = v.value();
    if (auto v = dto.tryGetValue<GenericDto>(TOKEN_ROOT)) quad_dto.root() = v.value();
    return quad_dto;
}

GenericDto QuadTreeRootDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_NAME, name());
    dto.addOrUpdate(TOKEN_ROOT, root());
    return dto;
}


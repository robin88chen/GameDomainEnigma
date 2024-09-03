#include "SceneGraphDtos.h"
#include "LazyNode.h"
#include "Node.h"
#include "Pawn.h"
#include "VisibilityManagedNode.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

static std::string TOKEN_ID = "Id";
static std::string TOKEN_CHILDREN = "Children";
static std::string TOKEN_NATIVE_DTO = "NativeDto";
static std::string TOKEN_PAWN_PRIMITIVE_ID = "PawnPrimitiveId";


/*NodeDto::ChildDto::ChildDto(const Engine::GenericDto& dto_from)
{
    if (auto v = dto_from.tryGetValue<std::vector<std::string>>(TOKEN_ID)) m_id = v.value();
    if (auto v = dto_from.tryGetValue<GenericDto>(TOKEN_NATIVE_DTO)) m_dto = v.value();
}

NodeDto::ChildDto::ChildDto(const SpatialId& id) : m_id(id)
{
}

NodeDto::ChildDto::ChildDto(const SpatialId& id, const Engine::GenericDto& child_dto) : m_id(id), m_dto(child_dto)
{
}

GenericDto NodeDto::ChildDto::toGenericDto() const
{
    GenericDto dto;
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    if (m_dto) dto.addOrUpdate(TOKEN_NATIVE_DTO, m_dto.value());
    return dto;
}

NodeDto::NodeDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Node::TYPE_RTTI.getName());
}

NodeDto::NodeDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Node::TYPE_RTTI.getName()));
}

NodeDto::NodeDto(const Engine::GenericDto& dto) : SpatialDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Node::TYPE_RTTI.getName()));
    if (auto children = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_CHILDREN))
    {
        for (const Engine::GenericDto& child : children.value())
        {
            m_children.emplace_back(child);
        }
    }
}

GenericDto NodeDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    Engine::GenericDtoCollection dtos;
    for (const ChildDto& child : m_children)
    {
        dtos.push_back(child.toGenericDto());
    }
    dto.addOrUpdate(TOKEN_CHILDREN, dtos);
    return dto;
}

std::vector<SpatialId> NodeDto::tokensToIds(const std::vector<std::string>& child_tokens)
{
    std::vector<SpatialId> child_ids;
    for (unsigned i = 0; i < child_tokens.size(); i += 2)
    {
        child_ids.push_back(SpatialId({ child_tokens[i], child_tokens[i + 1] }));
    };
    return child_ids;
}

std::vector<std::string> NodeDto::idsToTokens(const std::vector<SpatialId> child_ids) const
{
    std::vector<std::string> child_tokens;
    for (const SpatialId& id : child_ids)
    {
        auto tokens = id.tokens();
        child_tokens.insert(child_tokens.end(), tokens.begin(), tokens.end());
    }
    return child_tokens;
}*/


PawnDto::PawnDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Pawn::TYPE_RTTI.getName());
}

PawnDto::PawnDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Pawn::TYPE_RTTI.getName()));
}

PawnDto::PawnDto(const Engine::GenericDto& dto) : SpatialDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Pawn::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PAWN_PRIMITIVE_ID)) m_primitiveId = v.value();
}

GenericDto PawnDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    if (m_primitiveId)
    {
        dto.addOrUpdate(TOKEN_PAWN_PRIMITIVE_ID, m_primitiveId.value().tokens());
    }
    return dto;
}

/*LazyNodeDto::LazyNodeDto() : NodeDto()
{
    m_factoryDesc = FactoryDesc(LazyNode::TYPE_RTTI.getName());
}

LazyNodeDto::LazyNodeDto(const NodeDto& node_dto) : NodeDto(node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), LazyNode::TYPE_RTTI.getName()));
}

LazyNodeDto::LazyNodeDto(const Engine::GenericDto& dto) : NodeDto(dto)
{
}

GenericDto LazyNodeDto::toGenericDto() const
{
    GenericDto dto = NodeDto::toGenericDto();
    return dto;
}*/

VisibilityManagedNodeDto::VisibilityManagedNodeDto() : LazyNodeDto()
{
    m_factoryDesc = FactoryDesc(VisibilityManagedNode::TYPE_RTTI.getName());
}

VisibilityManagedNodeDto::VisibilityManagedNodeDto(const LazyNodeDto& lazy_node_dto) : LazyNodeDto(lazy_node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), VisibilityManagedNode::TYPE_RTTI.getName()));
}

VisibilityManagedNodeDto::VisibilityManagedNodeDto(const Engine::GenericDto& dto) : LazyNodeDto(dto)
{
}

GenericDto VisibilityManagedNodeDto::toGenericDto() const
{
    GenericDto dto = LazyNodeDto::toGenericDto();
    return dto;
}

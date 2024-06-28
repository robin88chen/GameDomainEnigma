#include "AnimatedPawnAssembler.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;

AnimatedPawnAssembler::AnimatedPawnAssembler(const SceneGraph::SpatialId& id) : m_pawnAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(AnimatedPawn::TYPE_RTTI.getName()));
    m_pawnAssembler.factory(m_dto.factoryDesc());
}

PawnAssembler& AnimatedPawnAssembler::pawn()
{
    return m_pawnAssembler;
}

AnimatedPawnAssembler& AnimatedPawnAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_pawnAssembler.factory(factory);
    return *this;
}

AnimatedPawnAssembler& AnimatedPawnAssembler::animationClipMap(const AnimationClipMapAssembler& clip)
{
    m_dto.animationClipMapDto() = clip.toGenericDto();
    return *this;
}

AnimatedPawnAssembler& AnimatedPawnAssembler::asNative(const std::string& file_at_path)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsNative(file_at_path);
    m_dto.factoryDesc(fd);
    m_pawnAssembler.factory(m_dto.factoryDesc());
    return *this;
}

AnimatedPawnAssembler& AnimatedPawnAssembler::byPrefab(const std::string& prefab_name)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimByPrefab(prefab_name);
    m_dto.factoryDesc(fd);
    m_pawnAssembler.factory(m_dto.factoryDesc());
    return *this;
}

AnimatedPawnDto AnimatedPawnAssembler::toAnimatedPawnDto()
{
    AnimatedPawnDto pawn_dto(m_pawnAssembler.toGenericDto());
    pawn_dto.id(m_dto.id());
    pawn_dto.animationClipMapDto() = m_dto.animationClipMapDto();
    pawn_dto.factoryDesc(m_dto.factoryDesc());
    return pawn_dto;
}

Enigma::Engine::GenericDto AnimatedPawnAssembler::toGenericDto()
{
    return toAnimatedPawnDto().toGenericDto();
}

std::shared_ptr<AnimatedPawn> AnimatedPawnAssembler::constitute()
{
    return std::dynamic_pointer_cast<AnimatedPawn>(std::make_shared<RequestSpatialConstitution>(m_id, toGenericDto())->dispatch());
}


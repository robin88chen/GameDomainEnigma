#include "AnimatedPawnAssembler.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "AnimatedPawn.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;

static std::string TOKEN_ANIMATION_CLIP_MAP = "AnimClipMap";
static std::string TOKEN_AVATAR_RECIPES = "AvatarRecipes";

AnimatedPawnAssembler::AnimatedPawnAssembler(const SceneGraph::SpatialId& id) : PawnAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(AnimatedPawn::TYPE_RTTI);
}

void AnimatedPawnAssembler::factory(const Engine::FactoryDesc& factory)
{
    PawnAssembler::factory(factory);
    if (m_prefab) m_factoryDesc.ClaimByPrefab(m_prefab.value());
}

void AnimatedPawnAssembler::persist(const std::string& filename, const std::string& path_id)
{
    PawnAssembler::persist(filename, path_id);
    m_prefab.reset();
}

void AnimatedPawnAssembler::animationClipMap(const std::shared_ptr<AnimationClipMapAssembler>& clip)
{
    m_clip = clip;
}

void AnimatedPawnAssembler::animationClipMap(const AnimationClipMap& clip)
{
    m_clip = std::make_shared<AnimationClipMapAssembler>();
    for (auto& [key, value] : clip)
    {
        m_clip->addClip(key, value);
    }
}

void AnimatedPawnAssembler::byPrefab(const std::string& prefab_name)
{
    m_prefab = prefab_name;
    m_factoryDesc.ClaimByPrefab(prefab_name);
}

Enigma::Engine::GenericDto AnimatedPawnAssembler::assemble() const
{
    Engine::GenericDto dto = PawnAssembler::assemble();
    if (m_clip) dto.addOrUpdate(TOKEN_ANIMATION_CLIP_MAP, m_clip->toGenericDto());
    dto.addOrUpdate(TOKEN_AVATAR_RECIPES, m_avatarRecipeDtos);
    return dto;
}

AnimatedPawnDisassembler::AnimatedPawnDisassembler() : PawnDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(AnimatedPawn::TYPE_RTTI);
}

void AnimatedPawnDisassembler::disassemble(const Engine::GenericDto& dto)
{
    PawnDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_ANIMATION_CLIP_MAP)) m_animationClipMapDto = v.value();
    if (auto v = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_AVATAR_RECIPES)) m_avatarRecipeDtos = v.value();
}

/*AnimatedPawnDto AnimatedPawnAssembler::toAnimatedPawnDto()
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
}*/


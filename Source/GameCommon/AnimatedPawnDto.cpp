#include "AnimatedPawnDto.h"
#include "AnimatedPawn.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;

static std::string TOKEN_ANIMATION_CLIP_MAP = "AnimClipMap";
static std::string TOKEN_AVATAR_RECIPES = "AvatarRecipes";

AnimatedPawnDto::AnimatedPawnDto(const SceneGraph::PawnDto& dto) : PawnDto(dto)
{
}

AnimatedPawnDto AnimatedPawnDto::FromGenericDto(const Engine::GenericDto& dto)
{
    AnimatedPawnDto pawn_dto(PawnDto::FromGenericDto(dto));
    if (auto v = dto.TryGetValue<Engine::GenericDto>(TOKEN_ANIMATION_CLIP_MAP)) pawn_dto.TheAnimationClipMapDto() = AnimationClipMapDto::FromGenericDto(v.value());
    if (auto v = dto.TryGetValue<std::vector<Engine::GenericDto>>(TOKEN_AVATAR_RECIPES)) pawn_dto.AvatarRecipeDtos() = v.value();
    return pawn_dto;
}

Enigma::Engine::GenericDto AnimatedPawnDto::ToGenericDto() const
{
    Engine::GenericDto dto = PawnDto::ToGenericDto();
    dto.AddRtti(Engine::FactoryDesc(AnimatedPawn::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_ANIMATION_CLIP_MAP, m_animationClipMapDto.ToGenericDto());
    dto.AddOrUpdate(TOKEN_AVATAR_RECIPES, m_avatarRecipeDtos);
    return dto;
}

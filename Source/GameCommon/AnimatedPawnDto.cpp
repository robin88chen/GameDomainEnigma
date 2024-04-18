#include "AnimatedPawnDto.h"
#include "AnimatedPawn.h"
#include <cassert>

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;

static std::string TOKEN_ANIMATION_CLIP_MAP = "AnimClipMap";
static std::string TOKEN_AVATAR_RECIPES = "AvatarRecipes";

AnimatedPawnDto::AnimatedPawnDto() : PawnDto()
{
    m_factoryDesc = Engine::FactoryDesc(AnimatedPawn::TYPE_RTTI.getName());
}

AnimatedPawnDto::AnimatedPawnDto(const Engine::GenericDto& dto) : PawnDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), AnimatedPawn::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_ANIMATION_CLIP_MAP)) m_animationClipMapDto = v.value();
    if (auto v = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_AVATAR_RECIPES)) m_avatarRecipeDtos = v.value();
}

AnimatedPawnDto::AnimatedPawnDto(const SceneGraph::PawnDto& dto) : PawnDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), AnimatedPawn::TYPE_RTTI.getName()));
}

Enigma::Engine::GenericDto AnimatedPawnDto::toGenericDto() const
{
    Engine::GenericDto dto = PawnDto::toGenericDto();
    if (m_animationClipMapDto) dto.addOrUpdate(TOKEN_ANIMATION_CLIP_MAP, m_animationClipMapDto.value());
    dto.addOrUpdate(TOKEN_AVATAR_RECIPES, m_avatarRecipeDtos);
    return dto;
}

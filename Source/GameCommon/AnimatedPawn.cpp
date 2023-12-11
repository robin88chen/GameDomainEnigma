#include "AnimatedPawn.h"
#include "Animators/AnimatorCommands.h"
#include "Frameworks/CommandBus.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include "AvatarRecipes.h"
#include "AnimatedPawnDto.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Animators;
using namespace Enigma::Engine;

DEFINE_RTTI(GameCommon, AnimatedPawn, Pawn);

AnimatedPawn::AnimatedPawn(const SpatialId& id) : Pawn(id)
{
    m_factoryDesc = FactoryDesc(AnimatedPawn::TYPE_RTTI.getName());
}

AnimatedPawn::AnimatedPawn(const SpatialId& id, const Engine::GenericDto& o) : Pawn(id, o)
{
    AnimatedPawnDto dto = AnimatedPawnDto::fromGenericDto(o);
    if (auto clip = dto.TheAnimationClipMapDto()) m_animationClipMap = AnimationClipMap(clip.value());
    for (auto& avatar_dto : dto.AvatarRecipeDtos())
    {
        m_avatarRecipeList.push_back(AvatarRecipe::CreateFromGenericDto(avatar_dto));
    }
}

AnimatedPawn::~AnimatedPawn()
{
    m_avatarRecipeList.clear();
}

GenericDto AnimatedPawn::serializeDto()
{
    AnimatedPawnDto dto(SerializePawnDto());
    dto.TheAnimationClipMapDto() = m_animationClipMap.serializeDto();
    for (auto& avatar_recipe : m_avatarRecipeList)
    {
        dto.AvatarRecipeDtos().push_back(avatar_recipe->serializeDto());
    }
    return dto.toGenericDto();
}

void AnimatedPawn::PlayAnimation(const std::string& name)
{
    if (m_animationClipMap.GetAnimationClipMap().empty()) return;
    if (!m_primitive) return;

    auto action_clip = m_animationClipMap.FindAnimationClip(name);
    if (!action_clip) return;

    std::list<std::shared_ptr<Animator>> anim_list;
    enumAnimatorListDeep(anim_list);
    if (anim_list.size() == 0) return;
    for (auto& anim : anim_list)
    {
        if (anim->typeInfo().isExactly(ModelPrimitiveAnimator::TYPE_RTTI))
        {
            Frameworks::CommandBus::post(std::make_shared<AddListeningAnimator>(anim));
            if (ModelPrimitiveAnimatorPtr model_ani = std::dynamic_pointer_cast<ModelPrimitiveAnimator, Animator>(anim))
            {
                model_ani->FadeInAnimation(0.3f, action_clip.value().get().GetClip());
            }
        }
    }
}

void AnimatedPawn::StopAnimation()
{
    if (!m_primitive) return;

    std::list<std::shared_ptr<Animator>> anim_list;
    enumAnimatorListDeep(anim_list);
    if (anim_list.size() == 0) return;
    for (auto& anim : anim_list)
    {
        if (anim->typeInfo().isExactly(ModelPrimitiveAnimator::TYPE_RTTI))
        {
            Frameworks::CommandBus::post(std::make_shared<RemoveListeningAnimator>(anim));
            if (ModelPrimitiveAnimatorPtr model_ani = std::dynamic_pointer_cast<ModelPrimitiveAnimator, Animator>(anim))
            {
                model_ani->StopAnimation();
            }
        }
    }
}

void AnimatedPawn::AddAvatarRecipe(const std::shared_ptr<AvatarRecipe>& recipe)
{
    m_avatarRecipeList.push_back(recipe);
}

void AnimatedPawn::BakeAvatarRecipes()
{
    for (auto& recipe : m_avatarRecipeList)
    {
        recipe->Bake(std::dynamic_pointer_cast<Pawn, Spatial>(shared_from_this()));
    }
}

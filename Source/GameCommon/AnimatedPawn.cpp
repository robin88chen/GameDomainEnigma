#include "AnimatedPawn.h"
#include "Frameworks/CommandBus.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "AvatarRecipes.h"
#include "AnimatedPawnAssembler.h"
#include "Animators/AnimatorCommands.h"
#include "Renderables/RenderableEvents.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::Animators;

DEFINE_RTTI(GameCommon, AnimatedPawn, Pawn);

AnimatedPawn::AnimatedPawn(const SpatialId& id) : Pawn(id)
{
    m_factoryDesc = FactoryDesc(AnimatedPawn::TYPE_RTTI.getName());
    registerHandlers();
}

/*AnimatedPawn::AnimatedPawn(const SpatialId& id, const Engine::GenericDto& o) : Pawn(id, o)
{
    AnimatedPawnDto dto(o);
    if (auto clip = dto.animationClipMapDto()) m_animationClipMap = AnimationClipMap(clip.value());
    for (auto& avatar_dto : dto.avatarRecipeDtos())
    {
        m_avatarRecipeList.push_back(AvatarRecipe::createFromGenericDto(avatar_dto));
    }
    registerHandlers();
}*/

AnimatedPawn::~AnimatedPawn()
{
    unregisterHandlers();
    m_avatarRecipeList.clear();
}

std::shared_ptr<AnimatedPawn> AnimatedPawn::create(const SceneGraph::SpatialId& id)
{
    return std::make_shared<AnimatedPawn>(id);
}

std::shared_ptr<SpatialAssembler> AnimatedPawn::assembler() const
{
    return std::make_shared<AnimatedPawnAssembler>(m_id);
}

void AnimatedPawn::assemble(const std::shared_ptr<SceneGraph::SpatialAssembler>& assembler)
{
    Pawn::assemble(assembler);
    if (auto animatedPawnAssembler = std::dynamic_pointer_cast<AnimatedPawnAssembler>(assembler))
    {
        animatedPawnAssembler->animationClipMap(m_animationClipMap);
        for (auto& avatar_recipe : m_avatarRecipeList)
        {
            animatedPawnAssembler->addAvatarRecipe(avatar_recipe);
        }
    }
}

std::shared_ptr<SpatialDisassembler> AnimatedPawn::disassembler() const
{
    return std::make_shared<AnimatedPawnDisassembler>();
}

void AnimatedPawn::disassemble(const std::shared_ptr<SceneGraph::SpatialDisassembler>& disassembler)
{
    Pawn::disassemble(disassembler);
    if (auto animatedPawnDisassembler = std::dynamic_pointer_cast<AnimatedPawnDisassembler>(disassembler))
    {
        m_animationClipMap = animatedPawnDisassembler->animationClipMap();
        m_avatarRecipeList.clear();
        for (auto& avatar_recipe : animatedPawnDisassembler->avatarRecipes())
        {
            m_avatarRecipeList.push_back(avatar_recipe);
        }
    }
}

/*std::shared_ptr<AnimatedPawn> AnimatedPawn::constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& o)
{
    auto pawn = std::make_shared<AnimatedPawn>(id, o);
    return pawn;
}

GenericDto AnimatedPawn::serializeDto()
{
    AnimatedPawnDto dto(SerializePawnDto());
    dto.animationClipMapDto() = m_animationClipMap.serializeDto();
    for (auto& avatar_recipe : m_avatarRecipeList)
    {
        dto.avatarRecipeDtos().push_back(avatar_recipe->serializeDto());
    }
    return dto.toGenericDto();
}*/

void AnimatedPawn::registerHandlers()
{
    m_onRenderablePrimitiveHydrated = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onRenderablePrimitiveHydrated(e); });
    Frameworks::EventPublisher::subscribe(typeid(RenderablePrimitiveHydrated), m_onRenderablePrimitiveHydrated);
}

void AnimatedPawn::unregisterHandlers()
{
    Frameworks::EventPublisher::unsubscribe(typeid(RenderablePrimitiveHydrated), m_onRenderablePrimitiveHydrated);
    m_onRenderablePrimitiveHydrated = nullptr;
}

void AnimatedPawn::playAnimation(const std::string& name)
{
    if (m_animationClipMap.animationClipMap().empty()) return;
    if (!m_primitive) return;

    auto action_clip = m_animationClipMap.findAnimationClip(name);
    if (!action_clip) return;

    std::list<std::shared_ptr<Animator>> anim_list;
    enumAnimatorListDeep(anim_list);
    if (anim_list.size() == 0) return;
    for (auto& anim : anim_list)
    {
        if (anim->typeInfo().isExactly(ModelPrimitiveAnimator::TYPE_RTTI))
        {
            Frameworks::CommandBus::enqueue(std::make_shared<AddListeningAnimator>(anim->id()));
            if (std::shared_ptr<ModelPrimitiveAnimator> model_ani = std::dynamic_pointer_cast<ModelPrimitiveAnimator, Animator>(anim))
            {
                model_ani->fadeInAnimation(0.3f, action_clip.value().get().clip());
            }
        }
    }
}

void AnimatedPawn::stopAnimation()
{
    if (!m_primitive) return;

    std::list<std::shared_ptr<Animator>> anim_list;
    enumAnimatorListDeep(anim_list);
    if (anim_list.size() == 0) return;
    for (auto& anim : anim_list)
    {
        if (anim->typeInfo().isExactly(ModelPrimitiveAnimator::TYPE_RTTI))
        {
            Frameworks::CommandBus::enqueue(std::make_shared<RemoveListeningAnimator>(anim->id()));
            if (std::shared_ptr<ModelPrimitiveAnimator> model_ani = std::dynamic_pointer_cast<ModelPrimitiveAnimator, Animator>(anim))
            {
                model_ani->stopAnimation();
            }
        }
    }
}

void AnimatedPawn::addAvatarRecipe(const std::shared_ptr<AvatarRecipe>& recipe)
{
    m_avatarRecipeList.push_back(recipe);
}

void AnimatedPawn::bakeAvatarRecipes()
{
    for (auto& recipe : m_avatarRecipeList)
    {
        recipe->bake(std::dynamic_pointer_cast<Pawn, Spatial>(shared_from_this()));
    }
}

void AnimatedPawn::onRenderablePrimitiveHydrated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    if (const auto ev = std::dynamic_pointer_cast<RenderablePrimitiveHydrated, Frameworks::IEvent>(e))
    {
        if (ev->id() != m_primitive->id()) return;
        bakeAvatarRecipes();
    }
}

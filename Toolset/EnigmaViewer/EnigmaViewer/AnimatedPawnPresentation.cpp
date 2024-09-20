#include "AnimatedPawnPresentation.h"

#include <SceneGraph/SceneGraphQueries.h>

#include "ViewerCommands.h"
#include "GameCommon/AnimatedPawnAssembler.h"
#include "MathLib/MathGlobal.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "Frameworks/CommandBus.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Animators/AnimatorCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/EventPublisher.h"
#include "ViewerEvents.h"

using namespace EnigmaViewer;
using namespace Enigma::SceneGraph;
using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Renderables;
using namespace Enigma::Frameworks;
using namespace Enigma::Animators;

AnimatedPawnPresentation::AnimatedPawnPresentation()
{
    m_pawn = nullptr;

}

AnimatedPawnPresentation::~AnimatedPawnPresentation()
{
    m_pawn = nullptr;
}

void AnimatedPawnPresentation::subscribeHandlers()
{
    m_onSpatialRemoved = std::make_shared<EventSubscriber>([this](const IEventPtr& e) { onSpatialRemoved(e); });
    EventPublisher::subscribe(typeid(SpatialRemoved), m_onSpatialRemoved);
    m_onSpatialConstituted = std::make_shared<EventSubscriber>([this](const IEventPtr& e) { onSpatialConstituted(e); });
    EventPublisher::subscribe(typeid(SpatialConstituted), m_onSpatialConstituted);
    m_onConstituteSpatialFailed = std::make_shared<EventSubscriber>([this](const IEventPtr& e) { onConstituteSpatialFailed(e); });
    EventPublisher::subscribe(typeid(ConstituteSpatialFailed), m_onConstituteSpatialFailed);
}

void AnimatedPawnPresentation::unsubscribeHandlers()
{
    EventPublisher::unsubscribe(typeid(SpatialRemoved), m_onSpatialRemoved);
    m_onSpatialRemoved = nullptr;
    EventPublisher::unsubscribe(typeid(SpatialConstituted), m_onSpatialConstituted);
    m_onSpatialConstituted = nullptr;
    EventPublisher::unsubscribe(typeid(ConstituteSpatialFailed), m_onConstituteSpatialFailed);
    m_onConstituteSpatialFailed = nullptr;
}

void AnimatedPawnPresentation::presentPawn(const Enigma::SceneGraph::SpatialId& pawn_id, const Enigma::Primitives::PrimitiveId& model_id, const Enigma::SceneGraph::SpatialId& scene_root_id)
{
    m_sceneRootId = scene_root_id;
    if (hasPawn())
    {
        m_removingPawnId = m_presentingPawnId;
        m_presentingModelId = model_id;
        m_presentingPawnId = pawn_id;
        removePawn(scene_root_id);
    }
    else
    {
        m_presentingModelId = model_id;
        m_presentingPawnId = pawn_id;
        assemblePawn();
    }
}

void AnimatedPawnPresentation::loadPawn(const Enigma::SceneGraph::SpatialId& pawn_id,
    const Enigma::SceneGraph::SpatialId& scene_root_id)
{
    m_sceneRootId = scene_root_id;
    if (hasPawn())
    {
        CommandBus::enqueue(std::make_shared<DetachNodeChild>(scene_root_id, m_pawn->id()));
        auto animator_id = m_pawn->getPrimitive()->animatorId();
        CommandBus::enqueue(std::make_shared<RemoveListeningAnimator>(animator_id));
        m_pawn = nullptr;
    }
    m_pawn = std::dynamic_pointer_cast<AnimatedPawn>(Pawn::queryPawn(pawn_id));
    if (!m_pawn) return;
    m_presentingPawnId = pawn_id;
    attachPawnToRoot();
    listenPawnAnimation();
    refreshPawnModelTree();
    refreshPawnAnimationClipMap();
}

void AnimatedPawnPresentation::removePawn(const Enigma::SceneGraph::SpatialId& scene_root_id)
{
    if (!m_pawn) return;
    CommandBus::enqueue(std::make_shared<DetachNodeChild>(scene_root_id, m_pawn->id()));
    auto animator_id = m_pawn->getPrimitive()->animatorId();
    CommandBus::enqueue(std::make_shared<RemoveListeningAnimator>(animator_id));
    CommandBus::enqueue(std::make_shared<RemoveSpatial>(m_pawn->id()));
    m_pawn = nullptr;
}

void AnimatedPawnPresentation::assemblePawn()
{
    AnimatedPawnAssembler assembler(m_presentingPawnId);
    assembler.persist(m_presentingPawnId.name() + ".pawn", "APK_PATH");
    if (m_presentingModelId.has_value()) assembler.primitiveId(m_presentingModelId.value());
    assembler.localTransform(Matrix4::IDENTITY);
    m_pawn = std::dynamic_pointer_cast<AnimatedPawn>(std::make_shared<RequestSpatialConstitution>(m_presentingPawnId, assembler.assemble())->dispatch());
    attachPawnToRoot();
    listenPawnAnimation();
    refreshPawnModelTree();
    refreshPawnAnimationClipMap();
    m_presentingModelId.reset();
}

void AnimatedPawnPresentation::attachPawnToRoot()
{
    Enigma::MathLib::Matrix4 mx = Enigma::MathLib::Matrix4::MakeRotationXTransform(-Enigma::MathLib::Math::HALF_PI);
    CommandBus::enqueue(std::make_shared<AttachNodeChild>(m_sceneRootId, m_pawn, mx));
}

void AnimatedPawnPresentation::listenPawnAnimation()
{
    auto animator_id = m_pawn->getPrimitive()->animatorId();
    if (const auto animator = std::dynamic_pointer_cast<ModelPrimitiveAnimator>(Animator::queryAnimator(animator_id)))
    {
        animator->playAnimation(Enigma::Renderables::AnimationClip{ 0.0f, 2.5f, Enigma::Renderables::AnimationClip::WarpMode::Loop, 0 });
    }
    CommandBus::enqueue(std::make_shared<AddListeningAnimator>(animator_id));
}

void AnimatedPawnPresentation::refreshPawnModelTree()
{
    if (!m_pawn) return;
    if (auto model = std::dynamic_pointer_cast<ModelPrimitive>(m_pawn->getPrimitive()))
    {
        CommandBus::enqueue(std::make_shared<RefreshModelNodeTree>(model));
    }
}

void AnimatedPawnPresentation::refreshPawnAnimationClipMap()
{
    if (!m_pawn) return;
    CommandBus::enqueue(std::make_shared<RefreshAnimationClipList>(m_pawn->animationClipMap()));
}

void AnimatedPawnPresentation::onSpatialRemoved(const Enigma::Frameworks::IEventPtr& e)
{
    const auto event = std::dynamic_pointer_cast<SpatialRemoved>(e);
    if (event == nullptr) return;
    if (event->id() != m_removingPawnId) return;
    if (m_presentingModelId)
    {
        assemblePawn();
    }
}

void AnimatedPawnPresentation::onSpatialConstituted(const Enigma::Frameworks::IEventPtr& e)
{
    const auto event = std::dynamic_pointer_cast<SpatialConstituted>(e);
    if (event == nullptr) return;
    if (event->id() != m_presentingPawnId) return;
    Enigma::Primitives::PrimitiveId model_id;
    if ((m_pawn) && (m_pawn->getPrimitive()))
    {
        model_id = m_pawn->getPrimitive()->id();
    }
    EventPublisher::enqueue(std::make_shared<ViewingPawnConstituted>(m_presentingPawnId, model_id));
}

void AnimatedPawnPresentation::onConstituteSpatialFailed(const Enigma::Frameworks::IEventPtr& e)
{
    const auto event = std::dynamic_pointer_cast<ConstituteSpatialFailed>(e);
    if (event == nullptr) return;
    if (event->id() != m_presentingPawnId) return;
    EventPublisher::enqueue(std::make_shared<ConstituteViewingPawnFailed>(m_presentingPawnId, event->error()));
}


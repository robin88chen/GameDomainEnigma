#include "AnimatedPawnPresentation.h"
#include "GameCommon/AnimatedPawnAssembler.h"
#include "MathLib/MathGlobal.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "Frameworks/CommandBus.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Animators/AnimatorCommands.h"

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

void AnimatedPawnPresentation::presentPawn(const Enigma::SceneGraph::SpatialId& pawn_id, const Enigma::Primitives::PrimitiveId& model_id, const Enigma::SceneGraph::SpatialId& scene_root_id)
{
    m_pawnId = pawn_id;
    m_presentingModelId = model_id;
    m_sceneRootId = scene_root_id;
    if (hasPawn())
    {
        removePawn(scene_root_id);
    }
    else
    {
        assemblePawn();
    }
}

void AnimatedPawnPresentation::removePawn(const Enigma::SceneGraph::SpatialId& scene_root_id)
{
    if (!m_pawn) return;
    CommandBus::post(std::make_shared<DetachNodeChild>(scene_root_id, m_pawn));
    auto animator_id = m_pawn->getPrimitive()->animatorId();
    CommandBus::post(std::make_shared<RemoveListeningAnimator>(animator_id));
    CommandBus::post(std::make_shared<RemoveSpatial>(m_pawn->id()));
    m_pawn = nullptr;
}

void AnimatedPawnPresentation::assemblePawn()
{
    AnimatedPawnAssembler assembler(m_pawnId);
    assembler.pawn().primitive(m_presentingModelId.value());
    assembler.pawn().spatial().localTransform(Matrix4::IDENTITY);
    m_pawn = assembler.constitute(Enigma::SceneGraph::PersistenceLevel::Repository);
    Enigma::MathLib::Matrix4 mx = Enigma::MathLib::Matrix4::MakeRotationXTransform(-Enigma::MathLib::Math::HALF_PI);
    CommandBus::post(std::make_shared<AttachNodeChild>(m_sceneRootId, m_pawn, mx));
    auto animator_id = m_pawn->getPrimitive()->animatorId();
    if (const auto animator = std::dynamic_pointer_cast<ModelPrimitiveAnimator>(Animator::queryAnimator(animator_id)))
    {
        animator->playAnimation(Enigma::Renderables::AnimationClip{ 0.0f, 2.5f, Enigma::Renderables::AnimationClip::WarpMode::Loop, 0 });
    }
    CommandBus::post(std::make_shared<AddListeningAnimator>(animator_id));
    m_presentingModelId.reset();
}


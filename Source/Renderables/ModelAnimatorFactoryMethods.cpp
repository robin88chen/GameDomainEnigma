#include "ModelAnimatorFactoryMethods.h"
#include "ModelPrimitiveAnimator.h"
#include "ModelAnimatorAssembler.h"

using namespace Enigma::Renderables;

std::shared_ptr<ModelPrimitiveAnimator> Enigma::Renderables::createModelPrimitiveAnimator(const Animators::AnimatorId& id)
{
    return std::make_shared<ModelPrimitiveAnimator>(id);
}

std::shared_ptr<ModelPrimitiveAnimator> Enigma::Renderables::constituteModelPrimitiveAnimator(const Animators::AnimatorId& id, const Engine::GenericDto& dto)
{
    auto animator = std::make_shared<ModelPrimitiveAnimator>(id);
    auto disassembler = animator->disassembler();
    disassembler->disassemble(dto);
    animator->disassemble(disassembler);
    return animator;
}

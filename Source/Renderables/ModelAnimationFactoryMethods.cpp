#include "ModelAnimationFactoryMethods.h"
#include "ModelAnimationAsset.h"
#include "ModelAnimationAssembler.h"

using namespace Enigma::Renderables;

std::shared_ptr<ModelAnimationAsset> Enigma::Renderables::createModelAnimationAsset(const Animators::AnimationAssetId& id)
{
    return std::make_shared<ModelAnimationAsset>(id);
}

std::shared_ptr<ModelAnimationAsset> Enigma::Renderables::constituteModelAnimationAsset(const Animators::AnimationAssetId& id, const Engine::GenericDto& dto)
{
    auto asset = std::make_shared<ModelAnimationAsset>(id);
    auto disassembler = asset->disassembler();
    disassembler->disassemble(dto);
    asset->disassemble(disassembler);
    return asset;
}

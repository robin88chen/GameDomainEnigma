#include "DaeAnimationParser.h"
#include "DaeParserErrors.h"
#include "DaeSchema.h"
#include "DaeAnimationAssetParser.h"
#include "Animators/AnimationAssetId.h"
#include "Animators/AnimatorId.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Renderables/ModelAnimationAssembler.h"
#include "Renderables/SkinOperatorAssembler.h"
#include "Renderables/ModelAnimatorAssembler.h"
#include "ViewerCommands.h"
#include <system_error>
#include <cassert>

using namespace EnigmaViewer;

#define TOKEN_LIB_ANIMATIONS "library_animations"
#define TOKEN_ANIMATION "animation"

DaeAnimationParser::DaeAnimationParser(const std::function<void(const std::string&)>& output_pipe)
{
    assert(output_pipe);
    m_outputPipe = output_pipe;
}

std::error_code DaeAnimationParser::parseAnimations(const pugi::xml_node& collada_root, const std::string& model_name)
{
    if (!collada_root) return ParserError::emptyRoot;

    pugi::xml_node anim_lib_node = collada_root.child(TOKEN_LIB_ANIMATIONS);
    if (!anim_lib_node)
    {
        m_outputPipe("has no animations lib");
        return ParserError::noAnimationsLib;
    }
    m_animationAssetId = Enigma::Animators::AnimationAssetId("animations/" + model_name);
    m_animatorId = Enigma::Animators::AnimatorId("animators/" + model_name, Enigma::Renderables::ModelPrimitiveAnimator::TYPE_RTTI);
    pugi::xml_node anim_name_node = anim_lib_node.child(TOKEN_ANIMATION);
    DaeAnimationAssetParser anim_asset_parser(m_outputPipe, m_animationAssetId);
    std::error_code error = anim_asset_parser.parseNamedAnimationAsset(anim_name_node);
    if (!error)
    {
        std::make_shared<PersistAnimationAssetDto>(m_animationAssetId, anim_asset_parser.getAnimationAssembler()->assemble())->execute();
    }
    return error;
}

void DaeAnimationParser::persistAnimator(const Enigma::Primitives::PrimitiveId& controlled_primitive)
{
    Enigma::Renderables::ModelAnimatorAssembler animator_assembler(m_animatorId);
    animator_assembler.animationAsset(m_animationAssetId);
    animator_assembler.controlledPrimitive(controlled_primitive);
    for (const auto& [skin_name, bone_names] : DaeSchema::getSkinBoneNames())
    {
        auto skin_prim = DaeSchema::getMeshIdFromMeshNodeName(skin_name);
        if (!skin_prim) continue;
        std::shared_ptr<Enigma::Renderables::SkinOperatorAssembler> operator_assembler = std::make_shared<Enigma::Renderables::SkinOperatorAssembler>();
        operator_assembler->operatedSkin(skin_prim.value());
        operator_assembler->bones(bone_names);
        animator_assembler.addSkinOperator(operator_assembler);
    }
    animator_assembler.asNative(m_animatorId.name() + ".animator@APK_PATH");
    std::make_shared<PersistAnimatorDto>(m_animatorId, animator_assembler.assemble())->execute();
}

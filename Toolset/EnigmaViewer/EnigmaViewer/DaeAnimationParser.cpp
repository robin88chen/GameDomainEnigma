#include "DaeAnimationParser.h"
#include "DaeParserErrors.h"
#include "DaeAnimationAssetParser.h"
#include "Animators/AnimationAssetId.h"
#include "Animators/AnimatorId.h"
#include "Renderables/ModelPrimitiveAnimator.h"
#include "Renderables/ModelAnimationAssembler.h"
#include <system_error>
#include <cassert>

using namespace EnigmaViewer;

#define TOKEN_LIB_ANIMATIONS "library_animations"
#define TOKEN_ANIMATION "animation"

DaeAnimationParser::DaeAnimationParser(const std::function<void(const std::string&)>& output_pipe, const std::shared_ptr<Enigma::Animators::AnimationAssetStoreMapper>& animation_store_mapper)
{
    assert(output_pipe);
    m_outputPipe = output_pipe;
    m_animationStoreMapper = animation_store_mapper;
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
        if (m_animationStoreMapper.lock())
        {
            m_animationStoreMapper.lock()->putAnimationAsset(m_animationAssetId, anim_asset_parser.getAnimationAssembler()->assemble());
        }
    }
    return error;
}

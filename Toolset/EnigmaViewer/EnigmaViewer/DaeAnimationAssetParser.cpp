#include "DaeAnimationAssetParser.h"
#include "DaeAnimationKeyParser.h"
#include "DaeParserErrors.h"
#include "Renderables/ModelAnimationAssembler.h"

using namespace EnigmaViewer;

#define TOKEN_ANIMATION "animation"

DaeAnimationAssetParser::DaeAnimationAssetParser(const std::function<void(const std::string&)>& output_pipe, const Enigma::Animators::AnimationAssetId& animation_asset_id)
{
    assert(output_pipe);
    m_outputPipe = output_pipe;
    m_animationAssetId = animation_asset_id;
}

std::error_code DaeAnimationAssetParser::parseNamedAnimationAsset(const pugi::xml_node& named_anim_node)
{
    if (!named_anim_node) return ParserError::noAnimationNameNode;

    m_animationAssembler = std::make_shared<Enigma::Renderables::ModelAnimationAssembler>(m_animationAssetId);
    pugi::xml_node anim_node = named_anim_node.child(TOKEN_ANIMATION);
    while (anim_node)
    {
        DaeAnimationKeyParser anim_key_parser(m_outputPipe);
        auto er = anim_key_parser.parseAnimationKeys(anim_node);
        if (!er)
        {
            m_animationAssembler->nodeSRT(anim_key_parser.getTargetMeshNodeName(), anim_key_parser.getAnimationTimeSRTAssembler());
        }
        else
        {
            m_outputPipe(er.message());
            return er;
        }
        anim_node = anim_node.next_sibling(TOKEN_ANIMATION);
    }
    m_animationAssembler->asAsset(m_animationAssetId.name(), m_animationAssetId.name() + ".ani", "APK_PATH");

    return ParserError::ok;
}


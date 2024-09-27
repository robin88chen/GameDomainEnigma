/*********************************************************************
 * \file   DaeAnimationAssetParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_ANIMATION_ASSET_PARSER_H
#define DAE_ANIMATION_ASSET_PARSER_H

#include "pugixml.hpp"
#include "Animators/AnimationAssetId.h"
#include "Renderables/ModelAnimationAssembler.h"
#include <system_error>
#include <functional>

namespace EnigmaViewer
{
    class DaeAnimationAssetParser
    {
    public:
        DaeAnimationAssetParser(const std::function<void(const std::string&)>& output_pipe, const Enigma::Animators::AnimationAssetId& animation_asset_id);

        std::error_code parseNamedAnimationAsset(const pugi::xml_node& named_anim_node);

        [[nodiscard]] const Enigma::Animators::AnimationAssetId& getAnimationAssetId() const { return m_animationAssetId; }
        [[nodiscard]] std::shared_ptr<Enigma::Animators::AnimationAssetAssembler> getAnimationAssembler() const { return m_animationAssembler; }

    protected:
        std::function<void(const std::string&)> m_outputPipe;
        Enigma::Animators::AnimationAssetId m_animationAssetId;
        std::shared_ptr<Enigma::Renderables::ModelAnimationAssembler> m_animationAssembler;
    };
}

#endif // DAE_ANIMATION_ASSET_PARSER_H

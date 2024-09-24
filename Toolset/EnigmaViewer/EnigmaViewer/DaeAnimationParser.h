/*********************************************************************
 * \file   DaeAnimationParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_ANIMATION_PARSER_H
#define DAE_ANIMATION_PARSER_H

#include "pugixml.hpp"
#include "Animators/AnimationAssetId.h"
#include "Animators/AnimatorId.h"
#include "Animators/AnimationAssetStoreMapper.h"
#include <system_error>
#include <string>
#include <functional>

namespace EnigmaViewer
{
    class DaeAnimationParser
    {
    public:
        DaeAnimationParser(const std::function<void(const std::string&)>& output_pipe, const std::shared_ptr<Enigma::Animators::AnimationAssetStoreMapper>& animation_store_mapper);

        [[nodiscard]] std::error_code parseAnimations(const pugi::xml_node& collada_root, const std::string& model_name);
        [[nodiscard]] const Enigma::Animators::AnimationAssetId& getAnimationAssetId() const { return m_animationAssetId; }
        [[nodiscard]] const Enigma::Animators::AnimatorId& getAnimatorId() const { return m_animatorId; }

    protected:
        std::function<void(const std::string&)> m_outputPipe;
        std::weak_ptr<Enigma::Animators::AnimationAssetStoreMapper> m_animationStoreMapper;

        Enigma::Animators::AnimationAssetId m_animationAssetId;
        Enigma::Animators::AnimatorId m_animatorId;
    };
}

#endif // DAE_ANIMATION_PARSER_H

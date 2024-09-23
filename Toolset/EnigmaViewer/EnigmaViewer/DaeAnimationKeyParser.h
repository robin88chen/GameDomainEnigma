/*********************************************************************
 * \file   DaeAnimationKeyParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_ANIMATION_KEY_PARSER_H
#define DAE_ANIMATION_KEY_PARSER_H

#include "pugixml.hpp"
#include "Renderables/AnimationTimeSRTAssembler.h"
#include <functional>
#include <system_error>

namespace EnigmaViewer
{
    class DaeAnimationKeyParser
    {
    public:
        DaeAnimationKeyParser(const std::function<void(const std::string&)>& output_pipe);

        std::error_code parseAnimationKeys(const pugi::xml_node& key_node);

        [[nodiscard]] const std::shared_ptr<Enigma::Renderables::AnimationTimeSRTAssembler>& getAnimationTimeSRTAssembler() const { return m_animationTimeSRTAssembler; }
        [[nodiscard]] const std::string& getTargetMeshNodeName() const { return m_targetMeshNodeName; }

    protected:
        std::error_code parseAnimationSample(const pugi::xml_node& sampler_node, const pugi::xml_node& key_node);
        std::error_code parseTimeValue(const pugi::xml_node& time_value_source);
        std::error_code parseAnimationMatrix(const pugi::xml_node& anim_matrix_source);
        std::error_code analyzeSRTKeys();

    protected:
        std::function<void(const std::string&)> m_outputPipe;
        std::shared_ptr<Enigma::Renderables::AnimationTimeSRTAssembler> m_animationTimeSRTAssembler;
        std::string m_targetMeshNodeName;

        std::vector<float> m_timeValues;
        std::vector<Enigma::MathLib::Matrix4> m_animMatrixs;
    };
}

#endif // DAE_ANIMATION_KEY_PARSER_H

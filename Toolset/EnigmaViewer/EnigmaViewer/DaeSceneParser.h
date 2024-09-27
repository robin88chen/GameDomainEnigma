/*********************************************************************
 * \file   DaeSceneParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_SCENE_PARSER_H
#define DAE_SCENE_PARSER_H

#include "Renderables/ModelPrimitiveAssembler.h"
#include <functional>
#include <string>
#include <system_error>
#include "pugixml.hpp"

namespace EnigmaViewer
{
    class DaeSceneParser
    {
    public:
        DaeSceneParser(const std::function<void(const std::string&)>& output_pipe);

        std::error_code parseScene(const pugi::xml_node& collada_root, const std::string& filename);

        void persistModel(const Enigma::Animators::AnimatorId& animator_id);

        [[nodiscard]] const std::string& getModelName() const { return m_modelName; }
        [[nodiscard]] const Enigma::Primitives::PrimitiveId& getModelId() const { return m_modelId; }
        [[nodiscard]] const std::shared_ptr<Enigma::Renderables::ModelPrimitiveAssembler>& getModelAssembler() const { return m_modelAssembler; }

    protected:
        std::error_code parseModelSceneNode(const pugi::xml_node& model_scene_node, const std::string& filename);

    protected:
        std::function<void(const std::string&)> m_outputPipe;
        std::string m_modelName;
        Enigma::Primitives::PrimitiveId m_modelId;
        std::shared_ptr<Enigma::Renderables::ModelPrimitiveAssembler> m_modelAssembler;
    };
}

#endif // DAE_SCENE_PARSER_H

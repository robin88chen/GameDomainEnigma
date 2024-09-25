/*********************************************************************
 * \file   DaeVertexWeightsParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_VERTEX_WEIGHTS_PARSER_H
#define DAE_VERTEX_WEIGHTS_PARSER_H

#include <string>
#include <functional>
#include <system_error>
#include "pugixml.hpp"

namespace EnigmaViewer
{
    class DaeVertexWeightsParser
    {
    public:
        DaeVertexWeightsParser(const std::function<void(const std::string&)>& output_pipe);

        std::error_code parseVertexWeights(const pugi::xml_node& skin_host_xml, const pugi::xml_node& vertex_weights_xml);

        void clearSplitedVertexWeights();
        void pushSplitedVertexIndex(unsigned index);

        [[nodiscard]] const std::vector<std::string>& getSkinBoneNames() const { return m_skinBoneNames; }
        [[nodiscard]] const std::vector<unsigned int>& getSplitedWeightIndices() const { return m_splitedWeightIndices; }
        [[nodiscard]] const std::vector<float>& getSplitedVertexWeights() const { return m_splitedVertexWeights; }

    protected:
        std::error_code parseJointNameSource(const pugi::xml_node& bone_names_xml);
        std::error_code parseWeightsArraySource(const pugi::xml_node& weights_array_xml);

    protected:
        std::function<void(const std::string&)> m_outputPipe;

        std::vector<std::string> m_skinBoneNames;

        std::vector<int> m_weightCounts;
        std::vector<unsigned int> m_weightPaletteIndices;
        std::vector<float> m_vertexWeights;
        std::vector<float> m_weightSource;

        std::vector<unsigned int> m_splitedWeightIndices;
        std::vector<float> m_splitedVertexWeights;
    };
}

#endif // DAE_VERTEX_WEIGHTS_PARSER_H

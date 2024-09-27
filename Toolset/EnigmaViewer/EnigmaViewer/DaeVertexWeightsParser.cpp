#include "DaeVertexWeightsParser.h"
#include "DaeParserErrors.h"
#include "DaeSchema.h"
#include <cassert>
#include <sstream>

using namespace EnigmaViewer;

#define TOKEN_INPUT "input"
#define TOKEN_SEMANTIC "semantic"
#define TOKEN_JOINT_SEMANTIC "JOINT"
#define TOKEN_WEIGHT_SEMANTIC "WEIGHT"
#define TOKEN_VERTEX_WEIGHT_COUNT "vcount"
#define TOKEN_JOINT_WEIGHT_PAIR "v"
#define TOKEN_SOURCE "source"
#define TOKEN_COUNT "count"
#define TOKEN_NAME_ARRAY "Name_array"
#define TOKEN_FLOAT_ARRAY "float_array"

#define MAX_WEIGHT_COUNT 4

DaeVertexWeightsParser::DaeVertexWeightsParser(const std::function<void(const std::string&)>& output_pipe)
    : m_outputPipe(output_pipe)
{
    assert(m_outputPipe);
}

std::error_code DaeVertexWeightsParser::parseVertexWeights(const pugi::xml_node& skin_host_xml, const pugi::xml_node& vertex_weights_xml)
{
    pugi::xml_node input_xml = vertex_weights_xml.child(TOKEN_INPUT);
    while (input_xml)
    {
        if (strcmp(input_xml.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_JOINT_SEMANTIC) == 0)
        {
            std::string source_id = &(input_xml.attribute(TOKEN_SOURCE).as_string()[1]);
            pugi::xml_node source_xml = DaeSchema::findChildNodeWithId(skin_host_xml, TOKEN_SOURCE, source_id);
            if (source_xml)
            {
                auto er = parseJointNameSource(source_xml);
                if (er) return er;
            }
        }
        else if (strcmp(input_xml.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_WEIGHT_SEMANTIC) == 0)
        {
            std::string source_id = &(input_xml.attribute(TOKEN_SOURCE).as_string()[1]);
            pugi::xml_node source_xml = DaeSchema::findChildNodeWithId(skin_host_xml, TOKEN_SOURCE, source_id);
            if (source_xml)
            {
                auto er = parseWeightsArraySource(source_xml);
                if (er) return er;
            }
        }
        input_xml = input_xml.next_sibling(TOKEN_INPUT);
    }
    pugi::xml_node weight_count_xml = vertex_weights_xml.child(TOKEN_VERTEX_WEIGHT_COUNT);
    if (!weight_count_xml)
    {
        m_outputPipe("no weight count define!!");
        return ParserError::noWeightCount;
    }
    int data_count = vertex_weights_xml.attribute(TOKEN_COUNT).as_int();
    int max_weight_count = 0;
    m_weightCounts.resize(data_count);
    std::stringstream ss(std::string(weight_count_xml.text().as_string()));
    for (int i = 0; i < data_count; i++)
    {
        ss >> m_weightCounts[i];
        if (m_weightCounts[i] > max_weight_count) max_weight_count = m_weightCounts[i];
    }
    if (max_weight_count > MAX_WEIGHT_COUNT)
    {
        m_outputPipe("(not supported) max weight count > 4!!");
        return ParserError::tooManyWeights;
    }
    pugi::xml_node influence_pair_xml = vertex_weights_xml.child(TOKEN_JOINT_WEIGHT_PAIR);
    if (!influence_pair_xml)
    {
        m_outputPipe("no influence pair!!");
        return ParserError::noInfluencePair;
    }
    m_weightPaletteIndices.resize(data_count, 0xffffffff);
    m_vertexWeights.resize(data_count * MAX_WEIGHT_COUNT, 0.0f);
    ss = std::stringstream(std::string(influence_pair_xml.text().as_string()));
    for (int i = 0; i < data_count; i++)
    {
        unsigned char p[4] = { 0xff, 0xff, 0xff, 0xff };
        float w[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        for (int j = 0; j < m_weightCounts[i]; j++)
        {
            unsigned int b_idx;
            ss >> b_idx;
            unsigned int w_index;
            ss >> w_index;
            if (j >= MAX_WEIGHT_COUNT) continue;  // 超過max weight count, 不處理
            p[j] = (unsigned char)b_idx;
            //ss >> p[j];
            m_vertexWeights[i * MAX_WEIGHT_COUNT + j] = m_weightSource[w_index];
        }
        m_weightPaletteIndices[i] = (unsigned int)(p[3] << 24) + (unsigned int)(p[2] << 16)
            + (unsigned int)(p[1] << 8) + (unsigned int)(p[0]);
    }
    return ParserError::ok;
}

void DaeVertexWeightsParser::clearSplitedVertexWeights()
{
    m_splitedWeightIndices.clear();
    m_splitedVertexWeights.clear();
}

void DaeVertexWeightsParser::pushSplitedVertexIndex(unsigned index)
{
    m_splitedWeightIndices.push_back(m_weightPaletteIndices[index]);
    m_splitedVertexWeights.push_back(m_vertexWeights[index * MAX_WEIGHT_COUNT]);
    m_splitedVertexWeights.push_back(m_vertexWeights[index * MAX_WEIGHT_COUNT + 1]);
    m_splitedVertexWeights.push_back(m_vertexWeights[index * MAX_WEIGHT_COUNT + 2]);
    m_splitedVertexWeights.push_back(m_vertexWeights[index * MAX_WEIGHT_COUNT + 3]);
}

std::error_code DaeVertexWeightsParser::parseJointNameSource(const pugi::xml_node& bone_names_xml)
{
    pugi::xml_node name_array_xml = bone_names_xml.child(TOKEN_NAME_ARRAY);
    if (!name_array_xml)
    {
        m_outputPipe("no bone name array define!!");
        return ParserError::noBoneNameArray;
    }
    int data_count = name_array_xml.attribute(TOKEN_COUNT).as_int();
    m_skinBoneNames.resize(data_count, "");
    std::stringstream ss(std::string(name_array_xml.text().as_string()));
    for (int i = 0; i < data_count; i++)
    {
        ss >> m_skinBoneNames[i];
    }
    for (std::string& joint : m_skinBoneNames)
    {
        joint = DaeSchema::getMeshNameFromJointId(joint);
    }
    return ParserError::ok;
}

std::error_code DaeVertexWeightsParser::parseWeightsArraySource(const pugi::xml_node& weights_array_xml)
{
    pugi::xml_node data_array_xml = weights_array_xml.child(TOKEN_FLOAT_ARRAY);
    if (!data_array_xml)
    {
        m_outputPipe("no weight array source define!!");
        return ParserError::noWeightsArray;
    }
    int data_count = data_array_xml.attribute(TOKEN_COUNT).as_int();
    m_weightSource.resize(data_count, 0.0f);
    std::stringstream ss(std::string(data_array_xml.text().as_string()));
    for (int i = 0; i < data_count; i++)
    {
        ss >> m_weightSource[i];
    }
    return ParserError::ok;
}

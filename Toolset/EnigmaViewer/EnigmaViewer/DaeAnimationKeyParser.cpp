#include "DaeAnimationKeyParser.h"
#include "DaeParserErrors.h"
#include "DaeSchema.h"
#include "Renderables/AnimationTimeSRTAssembler.h"
#include "Renderables/AnimationTimeSRT.h"
#include <sstream>
#include <cassert>

using namespace EnigmaViewer;

#define TOKEN_CHANNEL "channel"
#define TOKEN_INPUT "input"
#define TOKEN_SOURCE "source"
#define TOKEN_SEMANTIC "semantic"
#define TOKEN_SAMPLER "sampler"
#define TOKEN_INPUT_SEMANTIC "INPUT"
#define TOKEN_OUTPUT_SEMANTIC "OUTPUT"
#define TOKEN_FLOAT_ARRAY "float_array"
#define TOKEN_COUNT "count"
#define TOKEN_TARGET "target"
#define SUFFIX_ANIM_TRANSFORM "/transform"

DaeAnimationKeyParser::DaeAnimationKeyParser(const std::function<void(const std::string&)>& output_pipe)
{
    assert(output_pipe);
    m_outputPipe = output_pipe;
}

std::error_code DaeAnimationKeyParser::parseAnimationKeys(const pugi::xml_node& key_node)
{
    if (!key_node)
    {
        m_outputPipe("no animation key node");
        return ParserError::noAnimationKeyNode;
    }
    m_animationTimeSRTAssembler = std::make_shared<Enigma::Renderables::AnimationTimeSRTAssembler>();
    pugi::xml_node channel_node = key_node.child(TOKEN_CHANNEL);
    if (!channel_node)
    {
        m_outputPipe("animation has no channel!!");
        return ParserError::noAnimationChannel;
    }
    std::string sampler_source_id;
    if (channel_node.attribute(TOKEN_SOURCE))
    {
        sampler_source_id = &(channel_node.attribute(TOKEN_SOURCE).as_string()[1]);
    }
    if (sampler_source_id.length() == 0)
    {
        m_outputPipe("anim source id empty!!");
        return ParserError::noAnimationSamplerSource;
    }
    pugi::xml_node sampler_node = DaeSchema::findChildNodeWithId(key_node, TOKEN_SAMPLER, sampler_source_id);
    if (!sampler_node)
    {
        m_outputPipe("can't find anim sampler " + sampler_source_id + "!!");
        return ParserError::noAnimationSampler;
    }
    auto er = parseAnimationSample(sampler_node, key_node);
    if (er) return er;

    std::string target_matrix_id;
    if (channel_node.attribute(TOKEN_TARGET))
    {
        target_matrix_id = channel_node.attribute(TOKEN_TARGET).as_string();
    }
    if (target_matrix_id.length() == 0)
    {
        m_outputPipe("anim target empty!!");
        return ParserError::noAnimationTarget;
    }
    size_t pos = target_matrix_id.rfind(SUFFIX_ANIM_TRANSFORM);
    std::string target_mesh_node_id;
    if (pos != std::string::npos)
    {
        target_mesh_node_id = target_matrix_id.substr(0, pos);
    }
    if (target_mesh_node_id.length() == 0)
    {
        m_outputPipe("can't find target mesh node id from" + target_matrix_id + "!!");
        return ParserError::noAnimationTargetMesh;
    }
    m_targetMeshNodeName = DaeSchema::getNodeNameFromId(target_mesh_node_id);
    if (m_targetMeshNodeName.length() == 0)
    {
        m_outputPipe("can't find animation target mesh node of " + target_mesh_node_id + "!!");
        return ParserError::noAnimationTargetMesh;
    }
    m_outputPipe("parse animation for node " + m_targetMeshNodeName + " done.");

    return ParserError::ok;
}

std::error_code DaeAnimationKeyParser::parseAnimationSample(const pugi::xml_node& sampler_node, const pugi::xml_node& key_node)
{
    if (!sampler_node) return ParserError::noAnimationSampler;
    m_timeValues.clear();
    m_animMatrixs.clear();
    pugi::xml_node input_node = sampler_node.child(TOKEN_INPUT);
    while (input_node)
    {
        if (!input_node.attribute(TOKEN_SEMANTIC))
        {
            input_node = input_node.next_sibling(TOKEN_INPUT);
            continue;
        }
        if (strcmp(input_node.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_INPUT_SEMANTIC) == 0)
        {
            std::string source_id = &(input_node.attribute(TOKEN_SOURCE).as_string()[1]);
            pugi::xml_node time_value_source = DaeSchema::findChildNodeWithId(key_node, TOKEN_SOURCE, source_id);
            if (time_value_source)
            {
                auto er = parseTimeValue(time_value_source);
                if (er) return er;
            }
        }
        else if (strcmp(input_node.attribute(TOKEN_SEMANTIC).as_string(), TOKEN_OUTPUT_SEMANTIC) == 0)
        {
            std::string source_id = &(input_node.attribute(TOKEN_SOURCE).as_string()[1]);
            pugi::xml_node anim_matrix_source = DaeSchema::findChildNodeWithId(key_node, TOKEN_SOURCE, source_id);
            if (anim_matrix_source)
            {
                auto er = parseAnimationMatrix(anim_matrix_source);
                if (er) return er;
            }
        }
        input_node = input_node.next_sibling(TOKEN_INPUT);
    }
    auto er = analyzeSRTKeys();
    return er;
}

std::error_code DaeAnimationKeyParser::parseTimeValue(const pugi::xml_node& time_value_source)
{
    if (!time_value_source) return ParserError::noTimeValueSource;
    unsigned int count = 0;
    pugi::xml_node data_ary = time_value_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary)
    {
        m_outputPipe("time value source has no data!!");
        return ParserError::noTimeValueArray;
    }
    if (data_ary.attribute(TOKEN_COUNT))
    {
        count = data_ary.attribute(TOKEN_COUNT).as_uint();
    }
    if (count == 0)
    {
        m_outputPipe("time value count = 0 !!");
        return ParserError::noTimeValueArray;
    }
    m_timeValues.resize(count);
    std::stringstream ss(std::string(data_ary.text().as_string()));
    for (unsigned int i = 0; i < count; i++)
    {
        ss >> m_timeValues[i];
    }
    return ParserError::ok;
}

std::error_code DaeAnimationKeyParser::parseAnimationMatrix(const pugi::xml_node& anim_matrix_source)
{
    if (!anim_matrix_source) return ParserError::noAnimMatrixSource;
    unsigned int count = 0;
    pugi::xml_node data_ary = anim_matrix_source.child(TOKEN_FLOAT_ARRAY);
    if (!data_ary)
    {
        m_outputPipe("animation matrix source has no data!!");
        return ParserError::noAnimMatrixArray;
    }
    if (data_ary.attribute(TOKEN_COUNT))
    {
        count = data_ary.attribute(TOKEN_COUNT).as_uint() / 16;
    }
    if (count == 0)
    {
        m_outputPipe("animation matrix count = 0 !!");
        return ParserError::noAnimMatrixArray;
    }
    m_animMatrixs.resize(count);
    std::stringstream ss(std::string(data_ary.text().as_string()));
    for (unsigned int i = 0; i < count; i++)
    {
        for (unsigned int j = 0; j < 16; j++)
        {
            ss >> ((float*)m_animMatrixs[i])[j];
        }
    }
    return ParserError::ok;
}

std::error_code DaeAnimationKeyParser::analyzeSRTKeys()
{
    assert(m_animationTimeSRTAssembler);
    if (m_timeValues.empty()) return ParserError::noTimeValues;
    Enigma::Renderables::AnimationTimeSRT::ScaleKey scale_key;
    Enigma::Renderables::AnimationTimeSRT::TranslateKey trans_key;
    Enigma::Renderables::AnimationTimeSRT::RotationKey rot_key;
    Enigma::Renderables::AnimationTimeSRT::ScaleKeyVector scale_keys;
    Enigma::Renderables::AnimationTimeSRT::TranslateKeyVector translate_keys;
    Enigma::Renderables::AnimationTimeSRT::RotationKeyVector rotation_keys;

    unsigned sample_count = static_cast<unsigned>(m_timeValues.size());
    if (m_animMatrixs.size() != sample_count)
    {
        m_outputPipe("number of time values & matrixs not match!!");
        return ParserError::animMatrixCountMismatch;
    }

    scale_keys.reserve(sample_count);
    translate_keys.reserve(sample_count);
    rotation_keys.reserve(sample_count);

    Enigma::MathLib::Quaternion last_qt;
    Enigma::MathLib::Vector3 last_trans = Enigma::MathLib::Vector3::ZERO;
    Enigma::MathLib::Vector3 last_scale = Enigma::MathLib::Vector3(1.0f, 1.0f, 1.0f);

    for (unsigned int i = 0; i < sample_count; i++)
    {
        float time_value = m_timeValues[i];
        std::tie(scale_key.m_vecKey, rot_key.m_qtKey, trans_key.m_vecKey) = m_animMatrixs[i].UnMatrixSRT();
        // fix invalid key values
        if ((!std::isfinite(scale_key.m_vecKey.x())) || (!std::isfinite(scale_key.m_vecKey.y()))
            || (!std::isfinite(scale_key.m_vecKey.z())))
        {
            scale_key.m_vecKey = last_scale; //Vector3::ZERO;
            rot_key.m_qtKey = last_qt; //Quaternion::ZERO;
        }
        if ((!std::isfinite(trans_key.m_vecKey.x())) || (!std::isfinite(trans_key.m_vecKey.y()))
            || (!std::isfinite(trans_key.m_vecKey.z())))
        {
            trans_key.m_vecKey = last_trans; //Vector3::ZERO;
        }
        if (i > 0)
        {
            if (last_qt.dot(rot_key.m_qtKey) < 0)  // 這個東西是一個trick.... 我們如果確保兩個Quaternion之間夾角是最短路徑的話，算slerp時就不用強制取shortest path
            {
                rot_key.m_qtKey = -rot_key.m_qtKey;
            }
        }

        last_qt = rot_key.m_qtKey;
        last_scale = scale_key.m_vecKey;
        last_trans = trans_key.m_vecKey;

        if (i <= 1)  // 第一個key,第二個key先輸出
        {
            scale_key.m_time = time_value;
            scale_keys.push_back(scale_key);
            rot_key.m_time = time_value;
            rotation_keys.push_back(rot_key);
            trans_key.m_time = time_value;
            translate_keys.push_back(trans_key);
        }
        else
        {
            // 如果連續三個相同的key, 將這個key蓋掉上一個
            if ((scale_key.m_vecKey == scale_keys[scale_keys.size() - 1].m_vecKey)
                && (scale_key.m_vecKey == scale_keys[scale_keys.size() - 2].m_vecKey))
            {
                scale_keys[scale_keys.size() - 1].m_time = time_value;  // key相同，只要改時間就好
            }
            else // 不同的key, 新增
            {
                scale_key.m_time = time_value;
                scale_keys.push_back(scale_key);
            }

            // 如果連續三個相同的key, 將這個key蓋掉上一個
            if ((rot_key.m_qtKey == rotation_keys[rotation_keys.size() - 1].m_qtKey)
                && (rot_key.m_qtKey == rotation_keys[rotation_keys.size() - 2].m_qtKey))
            {
                rotation_keys[rotation_keys.size() - 1].m_time = time_value;  // key相同，只要改時間就好
            }
            else // 不同的key, 新增
            {
                rot_key.m_time = time_value;
                rotation_keys.push_back(rot_key);
            }

            // 如果連續三個相同的key, 將這個key蓋掉上一個
            if ((trans_key.m_vecKey == translate_keys[translate_keys.size() - 1].m_vecKey)
                && (trans_key.m_vecKey == translate_keys[translate_keys.size() - 2].m_vecKey))
            {
                translate_keys[translate_keys.size() - 1].m_time = time_value;  // key相同，只要改時間就好
            }
            else // 不同的key, 新增
            {
                trans_key.m_time = time_value;
                translate_keys.push_back(trans_key);
            }
        }
    }
    for (auto& key : scale_keys)
    {
        m_animationTimeSRTAssembler->addScaleKey(key);
    }
    for (auto& key : rotation_keys)
    {
        m_animationTimeSRTAssembler->addRotationKey(key);
    }
    for (auto& key : translate_keys)
    {
        m_animationTimeSRTAssembler->addTranslationKey(key);
    }
    return ParserError::ok;
}

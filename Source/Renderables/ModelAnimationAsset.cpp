#include "ModelAnimationAsset.h"
#include "ModelAnimationDtos.h"

using namespace Enigma::Renderables;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

DEFINE_RTTI(Renderables, ModelAnimationAsset, AnimationAsset);

ModelAnimationAsset::ModelAnimationAsset(const AnimationAssetId& id) : AnimationAsset(id)
{
    m_factoryDesc = FactoryDesc(ModelAnimationAsset::TYPE_RTTI.getName());
}

ModelAnimationAsset::ModelAnimationAsset(const Engine::AnimationAssetId& id, const GenericDto& dto) : AnimationAsset(id)
{
    ModelAnimationAssetDto model_dto(dto);
    m_factoryDesc = model_dto.factoryDesc();
    m_meshNodeKeyArray.reserve(model_dto.meshNodeNames().size());
    for (unsigned i = 0; i < model_dto.meshNodeNames().size(); i++)
    {
        addMeshNodeTimeSRTData(model_dto.meshNodeNames()[i], AnimationTimeSRT(model_dto.timeSRTs()[i]));
    }
}

ModelAnimationAsset::~ModelAnimationAsset()
{
    m_meshNodeKeyArray.clear();
}

GenericDto ModelAnimationAsset::serializeDto()
{
    ModelAnimationAssetDto dto;
    dto.id() = m_id;
    dto.factoryDesc() = m_factoryDesc;
    std::vector<std::string> names;
    GenericDtoCollection srts;
    for (auto& key : m_meshNodeKeyArray)
    {
        names.emplace_back(key.m_meshNodeName);
        srts.emplace_back(key.m_timeSRTData.serializeDto());
    }
    dto.meshNodeNames() = names;
    dto.timeSRTs() = srts;
    return dto.toGenericDto();
}

void ModelAnimationAsset::reserveCapacity(unsigned mesh_node_count)
{
    m_meshNodeKeyArray.reserve(mesh_node_count);
}

void ModelAnimationAsset::addMeshNodeTimeSRTData(const std::string& mesh_node_name, const AnimationTimeSRT& srt_data)
{
    m_meshNodeKeyArray.emplace_back(MeshNodeTimeSRTData(mesh_node_name, srt_data));
}

Matrix4 ModelAnimationAsset::calculateTransformMatrix(unsigned ani_node_index, float off_time)
{
    if (ani_node_index >= m_meshNodeKeyArray.size()) return Matrix4::IDENTITY;
    return m_meshNodeKeyArray[ani_node_index].m_timeSRTData.calculateTransformMatrix(off_time);
}

SRTValueTie ModelAnimationAsset::calculateLerpedSRT(unsigned ani_node_index, float off_time)
{
    if (ani_node_index >= m_meshNodeKeyArray.size()) return SRTValueTie();
    return m_meshNodeKeyArray[ani_node_index].m_timeSRTData.calculateLerpedSRT(off_time);
}

Matrix4 ModelAnimationAsset::calculateFadedTransformMatrix(unsigned ani_node_index, float off_time_a, float off_time_b, float weight_a)
{
    if (ani_node_index >= m_meshNodeKeyArray.size()) return Matrix4::IDENTITY;
    return m_meshNodeKeyArray[ani_node_index].m_timeSRTData.
        calculateFadedTransformMatrix(off_time_a, off_time_b, weight_a);
}

SRTValueTie ModelAnimationAsset::calculateFadedLerpedSRT(unsigned ani_node_index, float off_time_a, float off_time_b, float weight_a)
{
    if (ani_node_index >= m_meshNodeKeyArray.size()) return SRTValueTie();
    return m_meshNodeKeyArray[ani_node_index].m_timeSRTData.
        calculateFadedLerpedSRT(off_time_a, off_time_b, weight_a);
}

std::optional<unsigned> ModelAnimationAsset::findMeshNodeIndex(const std::string& node_name)
{
    if (m_meshNodeKeyArray.size() == 0) return std::nullopt;
    unsigned int count = static_cast<unsigned int>(m_meshNodeKeyArray.size());
    for (unsigned int m = 0; m < count; m++)
    {
        if (m_meshNodeKeyArray[m].m_meshNodeName == node_name) return m;
    }
    return std::nullopt;
}

float ModelAnimationAsset::getAnimationLengthInSecond()
{
    if (m_meshNodeKeyArray.size() == 0) return 0.0f;
    float ret_time = 0.0f;
    for (unsigned int i = 0; i < m_meshNodeKeyArray.size(); i++)
    {
        float key_length = m_meshNodeKeyArray[i].m_timeSRTData.getMaxAnimationTime();
        if (ret_time < key_length) ret_time = key_length;
    }
    return ret_time;
}

void ModelAnimationAsset::appendModelAnimationAsset(float offset_time, const std::shared_ptr<ModelAnimationAsset>& src_asset)
{
    if (!src_asset) return;

    unsigned int src_node_count = src_asset->getMeshNodeDataCount();
    if (!src_node_count) return;

    for (unsigned int i = 0; i < src_node_count; i++)
    {
        auto dest_node_index = findMeshNodeIndex(src_asset->m_meshNodeKeyArray[i].m_meshNodeName);
        if (!dest_node_index) continue;

        m_meshNodeKeyArray[dest_node_index.value()].m_timeSRTData.appendTranslateKeyVector(
            offset_time, src_asset->m_meshNodeKeyArray[i].m_timeSRTData.getTranslateKeyVector());
        m_meshNodeKeyArray[dest_node_index.value()].m_timeSRTData.appendRotationKeyVector(
            offset_time, src_asset->m_meshNodeKeyArray[i].m_timeSRTData.getRotationKeyVector());
        m_meshNodeKeyArray[dest_node_index.value()].m_timeSRTData.appendScaleKeyVector(
            offset_time, src_asset->m_meshNodeKeyArray[i].m_timeSRTData.getScaleKeyVector());
    }
}

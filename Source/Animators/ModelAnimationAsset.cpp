#include "ModelAnimationAsset.h"

using namespace Enigma::Animators;
using namespace Enigma::MathLib;

ModelAnimationAsset::ModelAnimationAsset(const std::string& name) : AnimationAsset(name)
{

}

ModelAnimationAsset::~ModelAnimationAsset()
{
    m_meshNodeKeyArray.clear();
}

void ModelAnimationAsset::ReserveCapacity(unsigned mesh_node_count)
{
    m_meshNodeKeyArray.reserve(mesh_node_count);
}

void ModelAnimationAsset::AddMeshNodeTimeSRTData(const std::string& mesh_node_name, const AnimationTimeSRT& srt_data)
{
    m_meshNodeKeyArray.emplace_back(MeshNodeTimeSRTData(mesh_node_name, srt_data));
}

Matrix4 ModelAnimationAsset::CalculateTransformMatrix(unsigned ani_node_index, float off_time)
{
    if (ani_node_index >= m_meshNodeKeyArray.size()) return Matrix4::IDENTITY;
    return m_meshNodeKeyArray[ani_node_index].m_timeSRTData.CalculateTransformMatrix(off_time);
}

SRTValueTie ModelAnimationAsset::CalculateLerpedSRT(unsigned ani_node_index, float off_time)
{
    if (ani_node_index >= m_meshNodeKeyArray.size()) return SRTValueTie();
    return m_meshNodeKeyArray[ani_node_index].m_timeSRTData.CalculateLerpedSRT(off_time);
}

Matrix4 ModelAnimationAsset::CalculateFadedTransformMatrix(unsigned ani_node_index, float off_time_a, float off_time_b, float weight_a)
{
    if (ani_node_index >= m_meshNodeKeyArray.size()) return Matrix4::IDENTITY;
    return m_meshNodeKeyArray[ani_node_index].m_timeSRTData.
        CalculateFadedTransformMatrix(off_time_a, off_time_b, weight_a);
}

SRTValueTie ModelAnimationAsset::CalculateFadedLerpedSRT(unsigned ani_node_index, float off_time_a, float off_time_b, float weight_a)
{
    if (ani_node_index >= m_meshNodeKeyArray.size()) return SRTValueTie();
    return m_meshNodeKeyArray[ani_node_index].m_timeSRTData.
        CalculateFadedLerpedSRT(off_time_a, off_time_b, weight_a);
}

std::optional<unsigned> ModelAnimationAsset::FindMeshNodeIndex(std::string& node_name)
{
    if (m_meshNodeKeyArray.size() == 0) return std::nullopt;
    unsigned int count = static_cast<unsigned int>(m_meshNodeKeyArray.size());
    for (unsigned int m = 0; m < count; m++)
    {
        if (m_meshNodeKeyArray[m].m_meshNodeName == node_name) return m;
    }
    return std::nullopt;
}

float ModelAnimationAsset::GetAnimationLengthInSecond()
{
    if (m_meshNodeKeyArray.size() == 0) return 0.0f;
    float ret_time = 0.0f;
    for (unsigned int i = 0; i < m_meshNodeKeyArray.size(); i++)
    {
        float key_length = m_meshNodeKeyArray[i].m_timeSRTData.GetMaxAnimationTime();
        if (ret_time < key_length) ret_time = key_length;
    }
    return ret_time;
}

void ModelAnimationAsset::AppendModelAnimationAsset(float offset_time, const std::shared_ptr<ModelAnimationAsset>& src_asset)
{
    if (!src_asset) return;

    unsigned int src_node_count = src_asset->GetMeshNodeDataCount();
    if (!src_node_count) return;

    for (unsigned int i = 0; i < src_node_count; i++)
    {
        auto dest_node_index = FindMeshNodeIndex(src_asset->m_meshNodeKeyArray[i].m_meshNodeName);
        if (!dest_node_index) continue;

        m_meshNodeKeyArray[dest_node_index.value()].m_timeSRTData.AppendTranslateKeyVector(
            offset_time, src_asset->m_meshNodeKeyArray[i].m_timeSRTData.GetTranslateKeyVector());
        m_meshNodeKeyArray[dest_node_index.value()].m_timeSRTData.AppendRotationKeyVector(
            offset_time, src_asset->m_meshNodeKeyArray[i].m_timeSRTData.GetRotationKeyVector());
        m_meshNodeKeyArray[dest_node_index.value()].m_timeSRTData.AppendScaleKeyVector(
            offset_time, src_asset->m_meshNodeKeyArray[i].m_timeSRTData.GetScaleKeyVector());
    }
}

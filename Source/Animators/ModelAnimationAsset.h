/*********************************************************************
 * \file   ModelAnimationAsset.h
 * \brief  model animation asset, value object, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _MODEL_ANIMATION_ASSET_H
#define _MODEL_ANIMATION_ASSET_H

#include "AnimationAsset.h"
#include "AnimationTimeSRT.h"
#include <vector>
#include <optional>

namespace Enigma::Animators
{
    class ModelAnimationAssetDto;

    class ModelAnimationAsset : public AnimationAsset
    {
        DECLARE_EN_RTTI
    protected:
        struct MeshNodeTimeSRTData
        {
            MeshNodeTimeSRTData() = default;
            MeshNodeTimeSRTData(const std::string& mesh_node_name, const AnimationTimeSRT& srt_data)
                : m_meshNodeName(mesh_node_name), m_timeSRTData(srt_data) {};

            std::string m_meshNodeName;
            AnimationTimeSRT m_timeSRTData;
        };
    public:
        ModelAnimationAsset(const std::string& name);
        ModelAnimationAsset(const ModelAnimationAssetDto& dto);
        ModelAnimationAsset(const ModelAnimationAsset&) = delete;
        ModelAnimationAsset(ModelAnimationAsset&&) = delete;
        virtual ~ModelAnimationAsset() override;
        ModelAnimationAsset& operator=(const ModelAnimationAsset&) = delete;
        ModelAnimationAsset& operator=(ModelAnimationAsset&&) = delete;

        ModelAnimationAssetDto SerializeDto();

        /** reserve data array capacity */
        void ReserveCapacity(unsigned int mesh_node_count);
        /** add SRT data */
        void AddMeshNodeTimeSRTData(const std::string& mesh_node_name, const AnimationTimeSRT& srt_data);

        /** calculate transform matrix */
        MathLib::Matrix4 CalculateTransformMatrix(unsigned int ani_node_index, float off_time);
        /** calculate lerped SRT */
        SRTValueTie CalculateLerpedSRT(unsigned int ani_node_index, float off_time);
        /** calculate faded transform matrix \n
        animation matrix = clip a's * weight_a + clip b's * (1.0 - weight_a)
        */
        MathLib::Matrix4 CalculateFadedTransformMatrix(unsigned int ani_node_index, float off_time_a, float off_time_b, float weight_a);
        /** calculate Faded Lerped SRT \n
        animation SRT = clip a's lerped SRT * weight_a + clip b's lerped SRT * (1.0 - weight_a)
        */
        SRTValueTie CalculateFadedLerpedSRT(unsigned int ani_node_index,
            float off_time_a, float off_time_b, float weight_a);

        /** find mesh node in data array, return array index */
        std::optional<unsigned> FindMeshNodeIndex(std::string& node_name);

        unsigned int GetMeshNodeDataCount() const { return static_cast<unsigned int>(m_meshNodeKeyArray.size()); };

        /** get animation length (in second) */
        virtual float GetAnimationLengthInSecond() override;

        /** append Model Animation Asset from src */
        void AppendModelAnimationAsset(float offset_time, const std::shared_ptr<ModelAnimationAsset>& src_asset);

    protected:
        std::vector<MeshNodeTimeSRTData> m_meshNodeKeyArray;
    };
}

#endif // _MODEL_ANIMATION_ASSET_H

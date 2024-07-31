/*****************************************************************
 * \file   SpatialAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef SPATIAL_ASSEMBLER_H
#define SPATIAL_ASSEMBLER_H

#include "SpatialId.h"
#include "SpatialDto.h"
#include "GameEngine/BoundingVolume.h"
#include "Spatial.h"

namespace Enigma::SceneGraph
{
    class SpatialAssembler
    {
    public:
        SpatialAssembler(const SpatialId& id);

        const SpatialId& id() const { return m_id; }

        SpatialAssembler& factory(const Engine::FactoryDesc& factory);
        SpatialAssembler& localTransform(const MathLib::Matrix4& local_transform);
        SpatialAssembler& worldTransform(const MathLib::Matrix4& world_transform);
        SpatialAssembler& modelBound(const Engine::BoundingVolume& model_bound);
        SpatialAssembler& topLevel(bool top_level);
        SpatialAssembler& cullingMode(Spatial::CullingMode cull_mode);
        SpatialAssembler& notifyFlags(Spatial::NotifyFlags notify_flags);
        SpatialAssembler& spatialFlags(Spatial::SpatialFlags spatial_flags);
        SpatialAssembler& graphDepth(unsigned graph_depth);

        Engine::BoundingVolume modelBound() const { return m_modelBound; }
        MathLib::Matrix4 localTransform() const { return m_dto.localTransform(); }
        MathLib::Matrix4 worldTransform() const { return m_dto.worldTransform(); }

        Engine::GenericDto toGenericDto() const;

    private:
        SpatialId m_id;
        SpatialDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };
}

#endif // SPATIAL_ASSEMBLER_H

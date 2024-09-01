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
        SpatialAssembler(const SpatialAssembler&) = default;
        SpatialAssembler(SpatialAssembler&&) noexcept = default;
        SpatialAssembler& operator=(const SpatialAssembler&) = default;
        SpatialAssembler& operator=(SpatialAssembler&&) noexcept = default;
        virtual ~SpatialAssembler() = default;

        void factory(const Engine::FactoryDesc& factory);
        void localTransform(const MathLib::Matrix4& local_transform);
        void worldTransform(const MathLib::Matrix4& world_transform);
        void modelBound(const Engine::BoundingVolume& model_bound);
        void topLevel(bool top_level) { m_isTopLevel = top_level; }
        void cullingMode(Spatial::CullingMode cull_mode) { m_cullingMode = cull_mode; }
        void notifyFlags(Spatial::NotifyFlags notify_flags) { m_notifyFlag = notify_flags; }
        void spatialFlags(Spatial::SpatialFlags spatial_flags) { m_spatialFlag = spatial_flags; }
        void graphDepth(unsigned graph_depth) { m_graphDepth = graph_depth; }
        void parentId(const SpatialId& parent_id) { m_parentId = parent_id; }
        void asNative(const std::string& file_at_path);

        virtual Engine::GenericDto assemble() const;

    protected:
        void calculateWorldBound();

    protected:
        SpatialId m_id;
        Engine::FactoryDesc m_factoryDesc;
        bool m_isTopLevel;
        MathLib::Matrix4 m_localTransform;
        MathLib::Matrix4 m_worldTransform;
        unsigned int m_graphDepth;
        Engine::BoundingVolume m_modelBound;
        Engine::BoundingVolume m_worldBound;
        Spatial::CullingMode m_cullingMode;
        Spatial::SpatialFlags m_spatialFlag;
        Spatial::NotifyFlags m_notifyFlag;
        std::optional<SpatialId> m_parentId;
        std::optional<std::string> m_nativeFileAtPath;
    };
    class SpatialDisassembler
    {
    public:
        SpatialDisassembler();
        SpatialDisassembler(const SpatialDisassembler&) = default;
        SpatialDisassembler(SpatialDisassembler&&) noexcept = default;
        SpatialDisassembler& operator=(const SpatialDisassembler&) = default;
        SpatialDisassembler& operator=(SpatialDisassembler&&) noexcept = default;
        virtual ~SpatialDisassembler() = default;

        [[nodiscard]] SpatialId id() const { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factory() const { return m_factoryDesc; }
        [[nodiscard]] bool topLevel() const { return m_isTopLevel; }
        [[nodiscard]] const MathLib::Matrix4& localTransform() const { return m_localTransform; }
        [[nodiscard]] const MathLib::Matrix4& worldTransform() const { return m_worldTransform; }
        [[nodiscard]] unsigned graphDepth() const { return m_graphDepth; }
        [[nodiscard]] const Engine::BoundingVolume& modelBound() const { return m_modelBound; }
        [[nodiscard]] const Engine::BoundingVolume& worldBound() const { return m_worldBound; }
        [[nodiscard]] Spatial::CullingMode cullingMode() const { return m_cullingMode; }
        [[nodiscard]] Spatial::SpatialFlags spatialFlags() const { return m_spatialFlag; }
        [[nodiscard]] Spatial::NotifyFlags notifyFlags() const { return m_notifyFlag; }
        [[nodiscard]] const std::optional<SpatialId>& parentId() const { return m_parentId; }

        virtual void disassemble(const Engine::GenericDto& dto);

    protected:
        SpatialId m_id;
        Engine::FactoryDesc m_factoryDesc;
        bool m_isTopLevel;
        MathLib::Matrix4 m_localTransform;
        MathLib::Matrix4 m_worldTransform;
        unsigned int m_graphDepth;
        Engine::BoundingVolume m_modelBound;
        Engine::BoundingVolume m_worldBound;
        Spatial::CullingMode m_cullingMode;
        Spatial::SpatialFlags m_spatialFlag;
        Spatial::NotifyFlags m_notifyFlag;
        std::optional<SpatialId> m_parentId;
    };
}

#endif // SPATIAL_ASSEMBLER_H

/*****************************************************************
 * \file   LightingPawnAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 ******************************************************************/
#ifndef LIGHTING_PAWN_ASSEMBLERS_H
#define LIGHTING_PAWN_ASSEMBLERS_H

#include "SceneGraph/PawnAssembler.h"

namespace Enigma::Rendering
{
    class LightQuadPawnAssembler : public SceneGraph::PawnAssembler
    {
    public:
        LightQuadPawnAssembler(const SceneGraph::SpatialId& id);

        void hostLightId(const SceneGraph::SpatialId& id) { m_hostLightId = id; }

        Engine::GenericDto assemble() const override;

    protected:
        std::optional<SceneGraph::SpatialId> m_hostLightId;
    };
    class LightQuadPawnDisassembler : public SceneGraph::PawnDisassembler
    {
    public:
        LightQuadPawnDisassembler();

        [[nodiscard]] std::optional<SceneGraph::SpatialId> hostLightId() const { return m_hostLightId; }

        void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<SceneGraph::SpatialId> m_hostLightId;
    };

    class LightVolumePawnAssembler : public SceneGraph::PawnAssembler
    {
    public:
        LightVolumePawnAssembler(const SceneGraph::SpatialId& id);

        void hostLightId(const SceneGraph::SpatialId& id) { m_hostLightId = id; }
        void presentCameraId(const SceneGraph::SpatialId& id) { m_cameraId = id; }

        Engine::GenericDto assemble() const override;

    protected:
        std::optional<SceneGraph::SpatialId> m_hostLightId;
        std::optional<SceneGraph::SpatialId> m_cameraId;
    };
    class LightVolumePawnDisassembler : public SceneGraph::PawnDisassembler
    {
    public:
        LightVolumePawnDisassembler();

        [[nodiscard]] std::optional<SceneGraph::SpatialId> hostLightId() const { return m_hostLightId; }
        [[nodiscard]] std::optional<SceneGraph::SpatialId> presentCameraId() const { return m_cameraId; }

        void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<SceneGraph::SpatialId> m_hostLightId;
        std::optional<SceneGraph::SpatialId> m_cameraId;
    };
}

#endif // LIGHTING_PAWN_ASSEMBLERS_H

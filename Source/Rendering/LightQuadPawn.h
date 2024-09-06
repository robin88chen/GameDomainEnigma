/*********************************************************************
 * \file   LightQuadPawn.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef LIGHT_QUAD_PAWN_H
#define LIGHT_QUAD_PAWN_H

#include "LightingPawn.h"

namespace Enigma::Rendering
{
    using error = std::error_code;
    class LightQuadPawn : public LightingPawn
    {
        DECLARE_EN_RTTI;
    public:
        LightQuadPawn(const SceneGraph::SpatialId& id);
        LightQuadPawn(const LightQuadPawn&) = delete;
        LightQuadPawn(LightQuadPawn&&) = delete;
        virtual ~LightQuadPawn() override;
        LightQuadPawn& operator=(const LightQuadPawn&) = delete;
        LightQuadPawn& operator=(LightQuadPawn&&) = delete;

        static std::shared_ptr<LightQuadPawn> create(const SceneGraph::SpatialId& id);

        virtual std::shared_ptr<SceneGraph::SpatialAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<SceneGraph::SpatialAssembler>& assembler) override;
        virtual std::shared_ptr<SceneGraph::SpatialDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<SceneGraph::SpatialDisassembler>& disassembler) override;

        /// 影響的光源就是自己所帶的光源
        virtual error _updateSpatialRenderState() override;

    protected:
        virtual void onLightInfoUpdated(const Frameworks::IEventPtr& e) override;
    };
}

#endif // LIGHT_QUAD_PAWN_H

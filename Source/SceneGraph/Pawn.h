/*********************************************************************
 * \file   Pawn.h
 * \brief  Scene Graph Pawn class, entity, shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef PAWN_H
#define PAWN_H

#include "Spatial.h"
#include "GameEngine/IRenderer.h"
#include "Primitives/Primitive.h"
#include "Animators/Animator.h"
#include <string>
#include <list>

namespace Enigma::SceneGraph
{

    class Pawn : public Spatial
    {
        DECLARE_EN_RTTI;
    public:
        Pawn(const SpatialId& id);
        Pawn(const Pawn&) = delete;
        Pawn(Pawn&&) = delete;
        virtual ~Pawn() override;
        Pawn& operator=(const Pawn&) = delete;
        Pawn& operator=(Pawn&&) = delete;

        virtual std::shared_ptr<SpatialAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<SpatialAssembler>& assembler) override;
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler) override;

        static std::shared_ptr<Pawn> queryPawn(const SpatialId& id);

        /** on cull visible, insert this object to culler */
        virtual error onCullingVisible(Culler* culler, bool noCull) override;
        virtual bool canVisited() override { return true; };
        /** if has primitive, then is renderable object */
        virtual bool isRenderable() override;
        /** insert primitive to renderer (if has any) */
        virtual error insertToRenderer(const Engine::IRendererPtr& render) override;

        /** set entity's primitive */
        void SetPrimitive(const std::shared_ptr<Primitives::Primitive>& prim);
        /** set primitive with primitive's rtti & geometry name, 還沒有用到的實作 */
        //void SetPrimitive(const Rtti& type_rtti, const std::string& geo_name);
        /** get primitive */
        const std::shared_ptr<Primitives::Primitive>& getPrimitive() const { return m_primitive; };

        /** calculate model bound */
        virtual void CalculateModelBound(bool axis_align);

        virtual error _updateLocalTransform(const MathLib::Matrix4& mxLocal) override;
        virtual error _updateWorldData(const MathLib::Matrix4& mxParentWorld) override;

        /** enum animator list deep, including geometry's animator */
        virtual void enumAnimatorListDeep(std::list<std::shared_ptr<Animators::Animator>>& resultList);

    protected:
        std::shared_ptr<Primitives::Primitive> m_primitive;
    };
}

#endif // PAWN_H

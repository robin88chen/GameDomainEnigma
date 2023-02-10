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
#include "GameEngine/Primitive.h"
#include <string>

namespace Enigma::SceneGraph
{
    class PawnDto;

    class Pawn : public Spatial
    {
    public:
        Pawn(const std::string& name);
        Pawn(const PawnDto& dto);
        Pawn(const Pawn&) = delete;
        Pawn(Pawn&&) = delete;
        virtual ~Pawn() override;
        Pawn& operator=(const Pawn&) = delete;
        Pawn& operator=(Pawn&&) = delete;

        /** on cull visible, insert this object to culler */
        virtual error OnCullingVisible(Culler* culler, bool noCull) override;
        virtual bool CanVisited() override { return true; };
        /** if has primitive, then is renderable object */
        virtual bool IsRenderable() override;
        /** insert primitive to renderer (if has any) */
        virtual error InsertToRenderer(const Engine::IRendererPtr& render) override;

        /** set entity's primitive */
        void SetPrimitive(const Engine::PrimitivePtr& prim);
        /** set primitive with primitive's rtti & geometry name, 還沒有用到的實作 */
        //void SetPrimitive(const Rtti& type_rtti, const std::string& geo_name);
        /** get primitive */
        const Engine::PrimitivePtr& GetPrimitive() const { return m_primitive; };

        /** calculate model bound */
        virtual void CalculateModelBound(bool axis_align);

        virtual error _UpdateLocalTransform(const MathLib::Matrix4& mxLocal) override;
        virtual error _UpdateWorldData(const MathLib::Matrix4& mxParentWorld) override;

        /** enum animator list deep, including geometry's animator */
        //todo : animators
        //virtual void EnumAnimatorListDeep(AnimatorList& resultList) override;

    protected:
        Engine::PrimitivePtr m_primitive;
    };
}

#endif // PAWN_H

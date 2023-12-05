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
#include "GameEngine/Animator.h"
#include <string>
#include <list>

namespace Enigma::SceneGraph
{
    class PawnDto;

    class Pawn : public Spatial
    {
        DECLARE_EN_RTTI;
    public:
        Pawn(const SpatialId& id);
        Pawn(const SpatialId& id, const Engine::GenericDto& dto);
        Pawn(const Pawn&) = delete;
        Pawn(Pawn&&) = delete;
        virtual ~Pawn() override;
        Pawn& operator=(const Pawn&) = delete;
        Pawn& operator=(Pawn&&) = delete;

        virtual Engine::GenericDto serializeDto() override;

        /** on cull visible, insert this object to culler */
        virtual error onCullingVisible(Culler* culler, bool noCull) override;
        virtual bool canVisited() override { return true; };
        /** if has primitive, then is renderable object */
        virtual bool isRenderable() override;
        /** insert primitive to renderer (if has any) */
        virtual error insertToRenderer(const Engine::IRendererPtr& render) override;

        /** set entity's primitive */
        void SetPrimitive(const Engine::PrimitivePtr& prim);
        /** set primitive with primitive's rtti & geometry name, 還沒有用到的實作 */
        //void SetPrimitive(const Rtti& type_rtti, const std::string& geo_name);
        /** get primitive */
        const Engine::PrimitivePtr& GetPrimitive() const { return m_primitive; };

        /** calculate model bound */
        virtual void CalculateModelBound(bool axis_align);

        virtual error _updateLocalTransform(const MathLib::Matrix4& mxLocal) override;
        virtual error _updateWorldData(const MathLib::Matrix4& mxParentWorld) override;

        /** enum animator list deep, including geometry's animator */
        virtual void EnumAnimatorListDeep(std::list<std::shared_ptr<Engine::Animator>>& resultList);

    protected:
        PawnDto SerializePawnDto();

    protected:
        Engine::PrimitivePtr m_primitive;
    };
    using PawnPtr = std::shared_ptr<Pawn>;
}

#endif // PAWN_H

/*********************************************************************
 * \file   SkinAnimationOperator.h
 * \brief  skin animation operator, value object, use instance
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _SKIN_ANIMATION_OPERATOR_H
#define _SKIN_ANIMATION_OPERATOR_H

#include "SkinMeshPrimitive.h"
#include "ModelPrimitive.h"
#include "ModelAnimatorDtos.h"
#include "GameEngine/FactoryDesc.h"

namespace Enigma::Renderables
{
    class SkinAnimationOperator
    {
        DECLARE_EN_RTTI_NON_BASE
    public:
        SkinAnimationOperator();
        SkinAnimationOperator(const SkinAnimationOperator& op);
        SkinAnimationOperator(SkinAnimationOperator&& op) noexcept;
        ~SkinAnimationOperator();
        SkinAnimationOperator& operator=(const SkinAnimationOperator& op);
        SkinAnimationOperator& operator=(SkinAnimationOperator&& op) noexcept;

        SkinOperatorDto serializeDto() const;

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        void linkSkinMeshPrimitive(const std::shared_ptr<Renderables::SkinMeshPrimitive>& prim, const std::vector<std::string>& boneNodeNames);
        void calculateNodeOffsetMatrix(const std::shared_ptr<Renderables::ModelPrimitive>& model, const MathLib::Matrix4& root_ref_trans);
        void linkNodeOffsetMatrix(const std::shared_ptr<Renderables::ModelPrimitive>& model, const std::vector<MathLib::Matrix4>& boneNodeOffsets);
        void updateSkinMeshBoneMatrix(const Renderables::MeshNodeTree& mesh_node_tree);

        std::shared_ptr<Renderables::SkinMeshPrimitive> getTargetSkinMeshPrimitive() const
        {
            if (!m_skinMeshPrim.expired()) return m_skinMeshPrim.lock();
            return nullptr;
        };

        void relinkClonedSkinMesh(const std::shared_ptr<Renderables::SkinMeshPrimitive>& prim);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::weak_ptr<Renderables::SkinMeshPrimitive> m_skinMeshPrim;
        std::vector<std::string> m_boneNodeNames;
        std::vector<MathLib::Matrix4> m_nodeOffsets;
        std::vector<std::optional<unsigned>> m_skinNodeIndexMapping;  ///< index : bone effect matrix index in skin mesh, element : node index in model primitive
    };
}

#endif // _SKIN_ANIMATION_OPERATOR_H

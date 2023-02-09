/*********************************************************************
 * \file   SkinAnimationOperator.h
 * \brief  skin animation operator, value object, use instance
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _SKIN_ANIMATION_OPERATOR_H
#define _SKIN_ANIMATION_OPERATOR_H

#include "Renderer/SkinMeshPrimitive.h"
#include "Renderer/ModelPrimitive.h"
#include "AnimatorDtos.h"

namespace Enigma::Animators
{
    class SkinAnimationOperator
    {
        DECLARE_EN_RTTI_NON_BASE
    public:
        SkinAnimationOperator();
        SkinAnimationOperator(const SkinAnimationOperator& op);
        SkinAnimationOperator(SkinAnimationOperator&& op);
        ~SkinAnimationOperator();
        SkinAnimationOperator& operator=(const SkinAnimationOperator& op);
        SkinAnimationOperator& operator=(SkinAnimationOperator&& op);

        SkinOperatorDto SerializeDto();

        void LinkSkinMeshPrimitive(const std::shared_ptr<Renderer::SkinMeshPrimitive>& prim, const std::vector<std::string>& boneNodeNames);
        void CalculateNodeOffsetMatrix(const std::shared_ptr<Renderer::ModelPrimitive>& model, const MathLib::Matrix4& root_ref_trans);
        void LinkNodeOffsetMatrix(const std::shared_ptr<Renderer::ModelPrimitive>& model, const std::vector<MathLib::Matrix4>& boneNodeOffsets);
        void UpdateSkinMeshBoneMatrix(const Renderer::MeshNodeTree& mesh_node_tree);

        std::shared_ptr<Renderer::SkinMeshPrimitive> GetTargetSkinMeshPrimitive() const
        {
            if (!m_skinMeshPrim.expired()) return m_skinMeshPrim.lock();
            return nullptr;
        };

        void ReLinkClonedSkinMesh(const std::shared_ptr<Renderer::SkinMeshPrimitive>& prim);

    protected:
        std::weak_ptr<Renderer::SkinMeshPrimitive> m_skinMeshPrim;
        std::vector<std::string> m_boneNodeNames;
        std::vector<MathLib::Matrix4> m_nodeOffsets;
        std::vector<std::optional<unsigned>> m_skinNodeIndexMapping;  ///< index : bone effect matrix index in skin mesh, element : node index in model primitive
    };
}

#endif // _SKIN_ANIMATION_OPERATOR_H

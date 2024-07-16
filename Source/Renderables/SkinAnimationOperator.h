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
        SkinAnimationOperator(const Engine::GenericDto& dto);
        SkinAnimationOperator(const SkinAnimationOperator& op);
        SkinAnimationOperator(SkinAnimationOperator&& op) noexcept;
        ~SkinAnimationOperator();
        SkinAnimationOperator& operator=(const SkinAnimationOperator& op);
        SkinAnimationOperator& operator=(SkinAnimationOperator&& op) noexcept;

        Engine::GenericDto serializeDto() const;

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        void updateSkinMeshBoneMatrix(const Renderables::MeshNodeTree& mesh_node_tree);

        void onAttachingMeshNodeTree(const MeshNodeTree& mesh_node_tree);
        void onDetachingMeshNodeTree();

    protected:
        std::shared_ptr<Renderables::SkinMeshPrimitive> cacheSkinMesh();
        MathLib::Matrix4 t_posNodeOffset(unsigned index, stdext::optional_ref<const MeshNode> mesh_node);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::optional<Primitives::PrimitiveId> m_skinMeshId;
        std::weak_ptr<Renderables::SkinMeshPrimitive> m_cachedSkinMesh;
        std::vector<std::string> m_boneNodeNames;
        std::vector<MathLib::Matrix4> m_nodeOffsets;
        std::vector<MathLib::Matrix4> m_t_posNodeOffsets;
        std::vector<std::optional<unsigned>> m_skinNodeIndexMapping;  ///< index : bone effect matrix index in skin mesh, element : node index in model primitive
    };
}

#endif // _SKIN_ANIMATION_OPERATOR_H

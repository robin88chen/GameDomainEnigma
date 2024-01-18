/*********************************************************************
 * \file   SkinMeshPrimitive.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _SKIN_MESH_PRIMITIVE_H
#define _SKIN_MESH_PRIMITIVE_H

#include "MeshPrimitive.h"
#include "MeshNode.h"
#include "GameEngine/EffectVariable.h"
#include <vector>

namespace Enigma::Renderables
{
    class SkinMeshPrimitive : public MeshPrimitive
    {
        DECLARE_EN_RTTI;
    public:
        SkinMeshPrimitive(const Primitives::PrimitiveId& id);
        SkinMeshPrimitive(const Primitives::PrimitiveId& id, const Engine::GenericDto& dto, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository);
        SkinMeshPrimitive(const SkinMeshPrimitive&) = delete;
        SkinMeshPrimitive(SkinMeshPrimitive&&) = delete;
        ~SkinMeshPrimitive() override;
        SkinMeshPrimitive& operator=(const SkinMeshPrimitive&) = delete;
        SkinMeshPrimitive& operator=(SkinMeshPrimitive&&) = delete;

        virtual Engine::GenericDto serializeDto() const override;

        void bindOwnerRootRefTransform(const MathLib::Matrix4& mx);
        const MathLib::Matrix4& getOwnerRootRefTransform() const { return m_ownerNodeRootRefTransform; }
        virtual void changeEffectMaterialInSegment(unsigned index, const std::shared_ptr<Engine::EffectMaterial>& effect) override;
        virtual void changeEffectMaterials(const EffectMaterialList& effects) override;

        void createBoneMatrixArray(unsigned int size);
        void updateBoneEffectMatrix(unsigned int idx, const MathLib::Matrix4& ref_mx);

        /** bind primitive bone matrix */
        void bindPrimitiveBoneMatrix();
        /** bind segment bone matrix */
        void bindSegmentBoneMatrix(unsigned int index);
        /** un-bind primitive bone matrix */
        void loosePrimitiveBoneMatrix();
        /** un-bind segment bone matrix */
        void looseSegmentBoneMatrix(unsigned int index);

    private:
        void assignBoneMatrix(Engine::EffectVariable& var);

    protected:
        /** effect matrix = node animation matrix * node offset matrix
            node offset matrix = some reference node root ref. matrix * (bone node root ref. matrix).inverse
            也就是 bone node 與某個參考基準 node 之間的 transform 差異
            參考基準 node : skin mesh 的 vertex 資料的參考原點
        */
        std::vector<MathLib::Matrix4> m_boneEffectMatrix;

        MathLib::Matrix4 m_ownerNodeRootRefTransform;
    };
}

#endif // _SKIN_MESH_PRIMITIVE_H

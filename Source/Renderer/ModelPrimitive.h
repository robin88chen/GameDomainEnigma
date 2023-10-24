/*********************************************************************
 * \file   ModelPrimitive.h
 * \brief  Model Primitive, value object, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_H
#define _MODEL_PRIMITIVE_H

#include "GameEngine/Primitive.h"
#include "MeshNodeTree.h"
#include "MeshPrimitive.h"
#include "GameEngine/Animator.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <list>

namespace Enigma::Renderer
{
    class ModelPrimitive : public Engine::Primitive
    {
        DECLARE_EN_RTTI;
    public:
        ModelPrimitive(const std::string& name);
        ModelPrimitive(const ModelPrimitive& prim);
        ModelPrimitive(ModelPrimitive&& prim) noexcept;
        ~ModelPrimitive() override;
        ModelPrimitive& operator=(const ModelPrimitive& prim);
        ModelPrimitive& operator=(ModelPrimitive&& prim) noexcept;

        virtual Engine::GenericDto SerializeDto() const override;

        const std::string& getName() const { return m_name; }

        const MeshNodeTree& GetMeshNodeTree() const { return m_nodeTree; };
        MeshNodeTree& GetMeshNodeTree() { return m_nodeTree; };
        unsigned GetMeshPrimitiveCount();

        /** get mesh primitive
        @param cached_index index in cached mesh primitive array
        */
        MeshPrimitivePtr GetMeshPrimitive(unsigned int cached_index);
        MeshPrimitivePtr FindMeshPrimitive(const std::string& name);
        /** get mesh node
        @param cached_index index in cached mesh primitive array
        */
        stdext::optional_ref<MeshNode> GetCachedMeshNode(unsigned int cached_index);

        void UpdateMeshNodeLocalTransform(unsigned int index, const MathLib::Matrix4& mxLocal);

        /** insert to renderer */
        virtual error InsertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
            const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState) override;
        /** remove from renderer */
        virtual error RemoveFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer) override;

        /** calculate bounding volume */
        virtual void CalculateBoundingVolume(bool axis_align) override;

        /** update world transform */
        virtual void UpdateWorldTransform(const MathLib::Matrix4& mxWorld) override;

        virtual void SelectVisualTechnique(const std::string& techniqueName) override;

        /** enum animator list deep, including geometry's animator */
        virtual void EnumAnimatorListDeep(std::list<std::shared_ptr<Engine::Animator>>& resultList) override;

    protected:
        /** sometimes we need re-cache */
        void CacheMeshPrimitive();

    protected:
        std::string m_name;
        MeshNodeTree m_nodeTree;
        std::vector<unsigned int> m_meshPrimitiveIndexCache;  ///< 記錄哪個index的mesh node擁有mesh primitive
    };
    using ModelPrimitivePtr = std::shared_ptr<ModelPrimitive>;
}

#endif // _MODEL_PRIMITIVE_H

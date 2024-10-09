/*********************************************************************
 * \file   ModelPrimitive.h
 * \brief  Model Primitive, value object, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MODEL_PRIMITIVE_H
#define _MODEL_PRIMITIVE_H

#include "Primitives/PrimitiveId.h"
#include "Primitives/Primitive.h"
#include "MeshNodeTree.h"
#include "Animators/Animator.h"
#include "Frameworks/EventSubscriber.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <list>
#include <system_error>

namespace Enigma::Renderables
{
    using error = std::error_code;
    class MeshPrimitive;
    class ModelPrimitiveAnimator;
    class ModelPrimitive : public Primitives::Primitive
    {
        DECLARE_EN_RTTI;
    public:
        ModelPrimitive(const Primitives::PrimitiveId& id);
        ModelPrimitive(const ModelPrimitive& prim) = delete;
        ModelPrimitive(ModelPrimitive&& prim) = delete;
        ~ModelPrimitive() override;
        ModelPrimitive& operator=(const ModelPrimitive& prim) = delete;
        ModelPrimitive& operator=(ModelPrimitive&& prim) = delete;

        static std::shared_ptr<ModelPrimitive> create(const Primitives::PrimitiveId& id);

        virtual std::shared_ptr<Primitives::PrimitiveAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<Primitives::PrimitiveAssembler>& assembler) const override;
        virtual std::shared_ptr<Primitives::PrimitiveDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<Primitives::PrimitiveDisassembler>& disassembler) override; ///< that's double

        void registerHandlers();
        void unregisterHandlers();

        const std::string& getName() const { return m_name; }

        const MeshNodeTree& getMeshNodeTree() const { return m_nodeTree; };
        MeshNodeTree& getMeshNodeTree() { return m_nodeTree; };
        unsigned getMeshPrimitiveCount();

        /** get mesh primitive
        @param cached_index index in cached mesh primitive array
        */
        std::shared_ptr<MeshPrimitive> getMeshPrimitive(unsigned int cached_index);
        std::shared_ptr<MeshPrimitive> findMeshPrimitive(const Primitives::PrimitiveId& id);
        /** get mesh node
        @param cached_index index in cached mesh primitive array
        */
        stdext::optional_ref<MeshNode> getCachedMeshNode(unsigned int cached_index);

        void updateMeshNodeLocalTransform(unsigned int index, const MathLib::Matrix4& mxLocal);

        /** insert to renderer */
        virtual error insertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
            const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState) override;
        /** remove from renderer */
        virtual error removeFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer) override;

        /** calculate bounding volume */
        virtual void calculateBoundingVolume(bool axis_align) override;

        /** update world transform */
        virtual void updateWorldTransform(const MathLib::Matrix4& mxWorld) override;

        virtual void selectVisualTechnique(const std::string& techniqueName) override;

        virtual const Animators::AnimatorId& animatorId() const override { return m_animatorId; }
        virtual void animatorId(const Animators::AnimatorId& animator_id) override;

        /** enum animator list deep, including geometry's animator */
        virtual void enumAnimatorListDeep(std::list<std::shared_ptr<Animators::Animator>>& resultList) override;

    protected:
        /** sometimes we need re-cache */
        void cacheMeshPrimitive();

        void onMeshHydrated(const Frameworks::IEventPtr& e);

        virtual std::shared_ptr<Animators::Animator> getAnimator() override;

    protected:
        std::string m_name;
        MeshNodeTree m_nodeTree;
        std::vector<unsigned int> m_meshPrimitiveIndexCache;  ///< 記錄哪個index的mesh node擁有mesh primitive

        std::shared_ptr<ModelPrimitiveAnimator> m_modelPrimitiveAnimator;

        Frameworks::EventSubscriberPtr m_onMeshHydrated;
    };
}

#endif // _MODEL_PRIMITIVE_H

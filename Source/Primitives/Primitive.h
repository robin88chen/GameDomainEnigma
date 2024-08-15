/*********************************************************************
 * \file   Primitive.h
 * \brief  primitive base class, aggregate, use shared_ptr, owned animator entity
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "GameEngine/RenderLightingState.h"
#include "GameEngine/BoundingVolume.h"
#include "PrimitiveId.h"
#include "GameEngine/IRenderer.h"
#include "MathLib/Matrix4.h"
#include "Frameworks/Rtti.h"
#include "Frameworks/LazyStatus.h"
#include "Animators/Animator.h"
#include <memory>
#include <bitset>
#include <system_error>
#include <list>

namespace Enigma::Primitives
{
    using error = std::error_code;
    class PrimitiveAssembler;

    class Primitive : public std::enable_shared_from_this<Primitive>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        enum PrimitiveBits
        {
            Primitive_UnBound = 0x01,       ///< 不計算Bounding的Primitive
            Primitive_UnRenderable = 0x02, ///< 不做 Rendering (i.e. Hide)
        };
        using PrimitiveFlags = std::bitset<2>;

    public:
        Primitive(const PrimitiveId& id);
        Primitive(const Primitive&) = delete;
        Primitive(Primitive&&) = delete;
        virtual ~Primitive();
        Primitive& operator=(const Primitive&) = delete;
        Primitive& operator=(Primitive&&) = delete;

        static std::shared_ptr<Primitive> queryPrimitive(const PrimitiveId& id);

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        const PrimitiveId& id() const { return m_id; }

        virtual std::shared_ptr<PrimitiveAssembler> assembler() const = 0;
        virtual void assemble(const std::shared_ptr<PrimitiveAssembler>& assembler) = 0; ///< that's double dispatch

        const Frameworks::LazyStatus& lazyStatus() const { return m_lazyStatus; }
        Frameworks::LazyStatus& lazyStatus() { return m_lazyStatus; }

        /** insert to renderer */
        virtual error insertToRendererWithTransformUpdating(const std::shared_ptr<Engine::IRenderer>& renderer,
            const MathLib::Matrix4& mxWorld, const Engine::RenderLightingState& lightingState) = 0;
        /** remove from renderer */
        virtual error removeFromRenderer(const std::shared_ptr<Engine::IRenderer>& renderer) = 0;

        /** get bounding volume */
        virtual const Engine::BoundingVolume& getBoundingVolume() { return m_bound; };
        /** calculate bounding volume */
        virtual void calculateBoundingVolume(bool axis_align) = 0;

        /** update world transform */
        virtual void updateWorldTransform(const MathLib::Matrix4& mxWorld) = 0;

        /** get current world transform */
        virtual const MathLib::Matrix4& getPrimitiveWorldTransform() { return m_mxPrimitiveWorld; };

        /** select visual technique */
        virtual void selectVisualTechnique(const std::string& techniqueName) { m_selectedVisualTech = techniqueName; };
        /** get selected visual technique */
        virtual std::string& getSelectedVisualTechnique() { return m_selectedVisualTech; };

        virtual const Animators::AnimatorId& animatorId() const { return m_animatorId; }
        virtual void animatorId(const Animators::AnimatorId& animator_id) { m_animatorId = animator_id; }

        /** enum animator list deep, including geometry's animator */
        virtual void enumAnimatorListDeep(std::list<std::shared_ptr<Animators::Animator>>& resultList);

        /** add primitive flag */
        void addPrimitiveFlag(PrimitiveFlags flag)
        {
            m_primitiveFlags |= flag;
        }
        /** remove primitive flag */
        void removePrimitiveFlag(PrimitiveFlags flag)
        {
            m_primitiveFlags &= (~flag);
        }
        /** get primitive flag */
        PrimitiveFlags getPrimitiveFlag() { return m_primitiveFlags; };
        /** test primitive flag */
        bool testPrimitiveFlag(PrimitiveFlags flag)
        {
            return (m_primitiveFlags & flag).any();
        }

    protected:
        virtual std::shared_ptr<Animators::Animator> getAnimator();

    protected:
        PrimitiveId m_id;
        Engine::FactoryDesc m_factoryDesc;
        Frameworks::LazyStatus m_lazyStatus;
        Engine::BoundingVolume m_bound;
        PrimitiveFlags m_primitiveFlags;
        MathLib::Matrix4 m_mxPrimitiveWorld;
        std::string m_selectedVisualTech;
        Animators::AnimatorId m_animatorId;
    };

    using PrimitivePtr = std::shared_ptr<Primitive>;
}

#endif // PRIMITIVE_H

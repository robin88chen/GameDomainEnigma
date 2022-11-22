/*********************************************************************
 * \file   Primitive.h
 * \brief  primitive base class, value object, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "RenderLightingState.h"
#include "BoundingVolume.h"
#include "MathLib/Matrix4.h"
#include "Frameworks/Rtti.h"
#include <memory>
#include <bitset>
#include <system_error>

namespace Enigma::Engine
{
    using error = std::error_code;

    class IRenderer;

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
        Primitive();
        Primitive(const Primitive&) = delete;
        Primitive(Primitive&&) = delete;
        virtual ~Primitive();
        Primitive& operator=(const Primitive&) = delete;
        Primitive& operator=(Primitive&&) = delete;

        /** insert to renderer */
        virtual error InsertToRendererWithTransformUpdating(const std::shared_ptr<IRenderer>& renderer,
            const MathLib::Matrix4& mxWorld, const RenderLightingState& lightingState) = 0;
        /** remove from renderer */
        virtual error RemoveFromRenderer(const std::shared_ptr<IRenderer>& renderer) = 0;

        /** get bounding volume */
        virtual const BoundingVolume& GetBoundingVolume() { return m_bound; };
        /** calculate bounding volume */
        virtual void CalculateBoundingVolume(bool axis_align) = 0;

        /** update world transform */
        virtual void UpdateWorldTransform(const MathLib::Matrix4& mxWorld) = 0;

    protected:
        BoundingVolume m_bound;
        PrimitiveFlags m_primitiveFlags;
        MathLib::Matrix4 m_mxPrimitiveWorld;
    };

    using PrimitivePtr = std::shared_ptr<Primitive>;
}

#endif // PRIMITIVE_H

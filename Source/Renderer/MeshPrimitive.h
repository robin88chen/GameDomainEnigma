/*********************************************************************
 * \file   MeshPrimitive.h
 * \brief Mesh primitive, value object, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_H
#define _MESH_PRIMITIVE_H

#include "GameEngine/Primitive.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/RenderBuffer.h"
#include <string>

namespace Enigma::Renderer
{
    class RenderElement;

    class MeshPrimitive : public Engine::Primitive
    {
        DECLARE_EN_RTTI;
    public:
        using EffectMaterialList = std::vector<Engine::EffectMaterialPtr>;
    public:
        MeshPrimitive(const std::string& name);
        MeshPrimitive(const MeshPrimitive&);
        MeshPrimitive(MeshPrimitive&&);
        ~MeshPrimitive() override;
        MeshPrimitive& operator=(const MeshPrimitive&);
        MeshPrimitive& operator=(MeshPrimitive&&);

        const std::string& GetName() const { return m_name; }
        /** get geometry data */
        const Engine::GeometryDataPtr& GetGeometryData() const { return m_geometry; };

        /** get effect */
        Engine::EffectMaterialPtr GetEffectMaterial(unsigned index);
        /** get material count */
        unsigned GetEffectMaterialCount() const;

        /** @name building mesh primitive */
        //@{
        /** link geometry object */
        void LinkGeometryData(const Engine::GeometryDataPtr& geo);
        /** change segment's effect */
        virtual void ChangeEffectMaterialInSegment(unsigned int index, const Engine::EffectMaterialPtr& effect);
        /** change primitive's effect */
        virtual void ChangeEffectMaterial(const EffectMaterialList& effects);
        /** resize effect list */
        void ResizeEffectMaterialVector(unsigned int new_size) { m_effects.resize(new_size); };
        //@}

    protected:
        void CleanupGeometry();

        /** bind primitive effect texture */
        void BindPrimitiveEffectTexture();
        /** bind segment effect texture */
        void BindSegmentEffectTexture(unsigned index);
        /** un-bind primitive effect texture */
        void LoosePrimitiveEffectTexture();
        /** un-bind segment effect texture */
        void LooseSegmentEffectTexture(unsigned index);

    protected:
        using RenderElementList = std::vector<std::shared_ptr<RenderElement>>;
        std::string m_name;
        Engine::GeometryDataPtr m_geometry;
        Engine::RenderBufferPtr m_renderBuffer;
        RenderElementList m_elements;
        EffectMaterialList m_effects;
    };
    using MeshPrimitivePtr = std::shared_ptr<MeshPrimitive>;
}

#endif // _MESH_PRIMITIVE_H

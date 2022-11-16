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
        MeshPrimitive(const std::string& name);
        MeshPrimitive(const MeshPrimitive&);
        MeshPrimitive(MeshPrimitive&&);
        ~MeshPrimitive() override;
        MeshPrimitive& operator=(const MeshPrimitive&);
        MeshPrimitive& operator=(MeshPrimitive&&);

        const std::string& GetName() const { return m_name; }
        /** get geometry data */
        const Engine::GeometryDataPtr& GetGeometryData() const { return m_geometry; };

    protected:
        using RenderElementList = std::vector<std::shared_ptr<RenderElement>>;
        std::string m_name;
        Engine::GeometryDataPtr m_geometry;
        Engine::RenderBufferPtr m_renderBuffer;
        RenderElementList m_elements;
    };
    using MeshPrimitivePtr = std::shared_ptr<MeshPrimitive>;
}

#endif // _MESH_PRIMITIVE_H

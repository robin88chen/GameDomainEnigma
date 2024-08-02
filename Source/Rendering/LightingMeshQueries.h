/*****************************************************************
 * \file   LightingMeshQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef LIGHTING_MESH_QUERIES_H
#define LIGHTING_MESH_QUERIES_H

#include "Frameworks/Query.h"
#include "Renderables/MeshPrimitive.h"

namespace Enigma::Rendering
{
    class RequestAmbientLightMeshAssembly : public Frameworks::Query<std::shared_ptr<Renderables::MeshPrimitive>>
    {
    public:
        RequestAmbientLightMeshAssembly(const Primitives::PrimitiveId& mesh_id) : m_meshId(mesh_id) {}

        const Primitives::PrimitiveId& meshId() const { return m_meshId; }

    protected:
        Primitives::PrimitiveId m_meshId;
    };
    class RequestSunLightMeshAssembly : public Frameworks::Query<std::shared_ptr<Renderables::MeshPrimitive>>
    {
    public:
        RequestSunLightMeshAssembly(const Primitives::PrimitiveId& mesh_id) : m_meshId(mesh_id) {}

        const Primitives::PrimitiveId& meshId() const { return m_meshId; }

    protected:
        Primitives::PrimitiveId m_meshId;
    };
    class RequestPointLightMeshAssembly : public Frameworks::Query<std::shared_ptr<Renderables::MeshPrimitive>>
    {
    public:
        RequestPointLightMeshAssembly(const Primitives::PrimitiveId& mesh_id, float sphere_radius) : m_meshId(mesh_id), m_sphereRadius(sphere_radius) {}

        const Primitives::PrimitiveId& meshId() const { return m_meshId; }
        float sphereRadius() const { return m_sphereRadius; }

    protected:
        Primitives::PrimitiveId m_meshId;
        float m_sphereRadius;
    };
}
#endif // LIGHTING_MESH_QUERIES_H

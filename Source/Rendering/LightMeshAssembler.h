/*********************************************************************
 * \file   LightMeshAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 *********************************************************************/
#ifndef LIGHT_MESH_ASSEMBLER_H
#define LIGHT_MESH_ASSEMBLER_H

#include "Frameworks/QuerySubscriber.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "Renderables/MeshPrimitive.h"

namespace Enigma::Rendering
{
    class DeferredRenderingConfiguration;
    class LightMeshAssembler
    {
    public:
        LightMeshAssembler(const std::shared_ptr<DeferredRenderingConfiguration>& configuration);
        LightMeshAssembler(const LightMeshAssembler&) = delete;
        LightMeshAssembler(LightMeshAssembler&&) = delete;
        ~LightMeshAssembler();
        LightMeshAssembler& operator=(const LightMeshAssembler&) = delete;
        LightMeshAssembler& operator=(LightMeshAssembler&&) = delete;

        void registerHandlers();
        void unregisterHandlers();

    protected:
        void requestAmbientLightMeshAssembly(const Frameworks::IQueryPtr& q);
        void requestSunLightMeshAssembly(const Frameworks::IQueryPtr& q);
        void requestPointLightMeshAssembly(const Frameworks::IQueryPtr& q);

        std::shared_ptr<Renderables::MeshPrimitive> assembleAmbientLightMesh(const Primitives::PrimitiveId& mesh_id);
        std::shared_ptr<Renderables::MeshPrimitive> assembleSunLightMesh(const Primitives::PrimitiveId& mesh_id);
        std::shared_ptr<Renderables::MeshPrimitive> assemblePointLightMesh(const Primitives::PrimitiveId& mesh_id, float sphere_radius);
        std::shared_ptr<Primitives::Primitive> assembleLightMesh(const Geometries::GeometryId& geometry_id, const Primitives::PrimitiveId& mesh_id, const Engine::EffectMaterialId& effect_material_id);
        Engine::EffectTextureMapAssembler getGBufferTextureSemantics();

    protected:
        std::shared_ptr<DeferredRenderingConfiguration> m_configuration;

        Frameworks::QuerySubscriberPtr m_requestAmbientLightMeshAssembly;
        Frameworks::QuerySubscriberPtr m_requestSunLightMeshAssembly;
        Frameworks::QuerySubscriberPtr m_requestPointLightMeshAssembly;
    };
}

#endif // LIGHT_MESH_ASSEMBLER_H

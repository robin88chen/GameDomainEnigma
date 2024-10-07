/*********************************************************************
 * \file   MeshPrimitiveBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_BUILDER_H
#define _MESH_PRIMITIVE_BUILDER_H

#include "Geometries/GeometryData.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectMaterial.h"
#include "Frameworks/EventSubscriber.h"
#include "Primitives/PrimitiveId.h"
#include "GameEngine/TextureId.h"
#include <memory>

namespace Enigma::Renderables
{
    class MeshPrimitive;
    class MeshPrimitiveDisassembler;
    class PrimitiveMaterial;

    class MeshPrimitiveBuilder
    {
    public:
        class MeshPrimitiveHydrated : public Frameworks::IEvent
        {
        public:
            MeshPrimitiveHydrated(const Primitives::PrimitiveId& id, const std::string& name, const std::shared_ptr<MeshPrimitive>& prim)
                : m_id(id), m_name(name), m_prim(prim) {};
            const Primitives::PrimitiveId& id() const { return m_id; }
            const std::string& name() const { return m_name; }
            const std::shared_ptr<MeshPrimitive>& primitive() { return m_prim; }

        private:
            Primitives::PrimitiveId m_id;
            std::string m_name;
            std::shared_ptr<MeshPrimitive> m_prim;
        };
        class MeshPrimitiveHydrationFailed : public Frameworks::IEvent
        {
        public:
            MeshPrimitiveHydrationFailed(const Primitives::PrimitiveId& id, const std::string& name, std::error_code er)
                : m_id(id), m_name(name), m_error(er) {};

            const Primitives::PrimitiveId& id() const { return m_id; }
            const std::string& name() const { return m_name; }
            std::error_code error() const { return m_error; }

        private:
            Primitives::PrimitiveId m_id;
            std::string m_name;
            std::error_code m_error;
        };
    public:
        MeshPrimitiveBuilder();
        MeshPrimitiveBuilder(const MeshPrimitiveBuilder&) = delete;
        MeshPrimitiveBuilder(MeshPrimitiveBuilder&&) = delete;
        ~MeshPrimitiveBuilder();
        MeshPrimitiveBuilder& operator=(const MeshPrimitiveBuilder&) = delete;
        MeshPrimitiveBuilder& operator=(MeshPrimitiveBuilder&&) = delete;

        void hydrateMeshPrimitive(const std::shared_ptr<MeshPrimitive>& mesh);

    protected:
        void onRenderBufferBuilt(const Frameworks::IEventPtr& e);
        void onBuildRenderBufferFailed(const Frameworks::IEventPtr& e);
        void onPrimitiveMaterialHydrated(const Frameworks::IEventPtr& e);
        void onPrimitiveMaterialHydrationFailed(const Frameworks::IEventPtr& e);

        void tryCompletingMesh();
        void failMeshHydration(const std::error_code er);

        bool isBuildingMeshMaterial(const std::shared_ptr<PrimitiveMaterial>& material) const;

        void cleanupBuildingMeta();

    protected:
        Primitives::PrimitiveId m_buildingId;
        //std::shared_ptr<MeshPrimitiveDisassembler> m_buildingDisassembler;

        std::shared_ptr<MeshPrimitive> m_builtPrimitive;
        std::shared_ptr<Geometries::GeometryData> m_builtGeometry;
        std::shared_ptr<Engine::RenderBuffer> m_builtRenderBuffer;

        Frameworks::EventSubscriberPtr m_onRenderBufferBuilt;
        Frameworks::EventSubscriberPtr m_onBuildRenderBufferFailed;
        Frameworks::EventSubscriberPtr m_onPrimitiveMaterialHydrated;
        Frameworks::EventSubscriberPtr m_onPrimitiveMaterialHydrationFailed;
    };
}

#endif // _MESH_PRIMITIVE_BUILDER_H

/*********************************************************************
 * \file   MeshPrimitiveBuilder.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_PRIMITIVE_BUILDER_H
#define _MESH_PRIMITIVE_BUILDER_H

#include "RenderablePrimitivePolicies.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectMaterial.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/EffectTextureMap.h"
#include "MeshPrimitive.h"
#include <memory>

namespace Enigma::Renderer
{
    class MeshPrimitiveBuilder
    {
    public:
        class MeshPrimitiveBuilt : public Frameworks::IEvent
        {
        public:
            MeshPrimitiveBuilt(const Frameworks::Ruid& ruid, const std::string& name, const std::shared_ptr<MeshPrimitive>& prim)
                : m_ruid(ruid), m_name(name), m_prim(prim) {};
            const Frameworks::Ruid& GetRuid() const { return m_ruid; }
            const std::string& GetName() const { return m_name; }
            const std::shared_ptr<MeshPrimitive>& GetPrimitive() { return m_prim; }

        private:
            Frameworks::Ruid m_ruid;
            std::string m_name;
            std::shared_ptr<MeshPrimitive> m_prim;
        };
        class BuildMeshPrimitiveFailed : public Frameworks::IEvent
        {
        public:
            BuildMeshPrimitiveFailed(const Frameworks::Ruid& ruid, const std::string& name, std::error_code er)
                : m_ruid(ruid), m_name(name), m_error(er) {};

            const Frameworks::Ruid& GetRuid() const { return m_ruid; }
            const std::string& GetName() const { return m_name; }
            std::error_code GetErrorCode() const { return m_error; }

        private:
            Frameworks::Ruid m_ruid;
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

        void BuildMeshPrimitive(const Frameworks::Ruid& ruid, const std::shared_ptr<MeshPrimitivePolicy>& policy);

    protected:
        void OnGeometryDataBuilt(const Frameworks::IEventPtr& e);
        void OnBuildGeometryDataFailed(const Frameworks::IEventPtr& e);
        void OnRenderBufferBuilt(const Frameworks::IEventPtr& e);
        void OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e);

        void OnEffectMaterialCompiled(const Frameworks::IEventPtr& e);
        void OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e);
        void OnTextureLoadedOrCreated(const Frameworks::IEventPtr& e);
        void OnLoadOrCreateTextureFailed(const Frameworks::IEventPtr& e);

        void TryCompletingMesh();

        std::optional<unsigned> FindBuildingEffectIndex(const std::string& name);
        std::optional<std::tuple<unsigned, unsigned>> FindLoadingTextureIndex(const std::string& name);

    protected:
        Frameworks::Ruid m_buildingRuid;
        std::shared_ptr<MeshPrimitivePolicy> m_policy;

        std::shared_ptr<MeshPrimitive> m_builtPrimitive;
        std::shared_ptr<Engine::GeometryData> m_builtGeometry;
        Engine::FactoryDesc m_originalGeometryDesc;
        std::shared_ptr<Engine::RenderBuffer> m_builtRenderBuffer;
        std::vector<std::shared_ptr<Engine::EffectMaterial>> m_builtEffects;
        std::vector<Engine::EffectTextureMap> m_builtTextures;

        Frameworks::EventSubscriberPtr m_onGeometryDataBuilt;
        Frameworks::EventSubscriberPtr m_onBuildGeometryDataFailed;
        Frameworks::EventSubscriberPtr m_onRenderBufferBuilt;
        Frameworks::EventSubscriberPtr m_onBuildRenderBufferFailed;

        Frameworks::EventSubscriberPtr m_onEffectMaterialCompiled;
        Frameworks::EventSubscriberPtr m_onCompileEffectMaterialFailed;
        Frameworks::EventSubscriberPtr m_onTextureLoaded;
        Frameworks::EventSubscriberPtr m_onLoadTextureFailed;
        Frameworks::EventSubscriberPtr m_onTextureCreated;
        Frameworks::EventSubscriberPtr m_onCreateTextureFailed;
    };
}

#endif // _MESH_PRIMITIVE_BUILDER_H

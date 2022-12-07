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
#include "GameEngine/Primitive.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/RenderBuffer.h"
#include "GameEngine/EffectMaterial.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/EffectTextureMap.h"
#include "MeshPrimitive.h"

namespace Enigma::Renderer
{
    class MeshPrimitiveBuilder
    {
    public:
        MeshPrimitiveBuilder();
        MeshPrimitiveBuilder(const MeshPrimitiveBuilder&) = delete;
        MeshPrimitiveBuilder(MeshPrimitiveBuilder&&) = delete;
        ~MeshPrimitiveBuilder();
        MeshPrimitiveBuilder& operator=(const MeshPrimitiveBuilder&) = delete;
        MeshPrimitiveBuilder& operator=(MeshPrimitiveBuilder&&) = delete;

        void BuildMeshPrimitive(const MeshPrimitivePolicy& policy);

    protected:
        void OnGeometryDataBuilt(const Frameworks::IEventPtr& e);
        void OnBuildGeometryDataFailed(const Frameworks::IEventPtr& e);
        void OnRenderBufferBuilt(const Frameworks::IEventPtr& e);
        void OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e);
        void OnEffectMaterialCompiled(const Frameworks::IEventPtr& e);
        void OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e);
        void OnTextureLoaded(const Frameworks::IEventPtr& e);
        void OnLoadTextureFailed(const Frameworks::IEventPtr& e);

        void TryCompletingMesh();

        std::optional<unsigned> FindBuildingEffectIndex(const std::string& name);
        std::optional<std::tuple<unsigned, unsigned>> FindLoadingTextureIndex(const std::string& name);

    protected:
        MeshPrimitivePolicy m_policy;

        std::shared_ptr<MeshPrimitive> m_builtPrimitive;
        std::shared_ptr<Engine::GeometryData> m_builtGeometry;
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
    };
}

#endif // _MESH_PRIMITIVE_BUILDER_H

#include <memory>
#include "MeshPrimitiveBuilder.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ResponseBus.h"
#include "GameEngine/GeometryCommands.h"
#include "GameEngine/GeometryDataEvents.h"
#include "GameEngine/RenderBufferCommands.h"
#include "GameEngine/RenderBufferEvents.h"
#include "GameEngine/EffectCommands.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
#include "GameEngine/FactoryCommands.h"

using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

MeshPrimitiveBuilder::MeshPrimitiveBuilder() : m_originalGeometryDesc(GeometryData::TYPE_RTTI.GetName()), m_buildingRuid()
{
    m_onGeometryDataBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnGeometryDataBuilt(e); });
    EventPublisher::Subscribe(typeid(GeometryDataBuilt), m_onGeometryDataBuilt);
    m_onBuildGeometryDataFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildGeometryDataFailed(e); });
    EventPublisher::Subscribe(typeid(BuildGeometryDataFailed), m_onBuildGeometryDataFailed);
    m_onRenderBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderBufferBuilt(e); });
    EventPublisher::Subscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onBuildRenderBufferFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildRenderBufferFailed(e); });
    EventPublisher::Subscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);

    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    EventPublisher::Subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnCompileEffectMaterialFailed(e); });
    EventPublisher::Subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);

    m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoadedOrCreated(e); });
    EventPublisher::Subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnLoadOrCreateTextureFailed(e); });
    EventPublisher::Subscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
    m_onTextureCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoadedOrCreated(e); });
    EventPublisher::Subscribe(typeid(TextureCreated), m_onTextureCreated);
    m_onCreateTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnLoadOrCreateTextureFailed(e); });
    EventPublisher::Subscribe(typeid(CreateTextureFailed), m_onCreateTextureFailed);

    CommandBus::Post(std::make_shared<RegisterDtoPolicyConverter>(MeshPrimitive::TYPE_RTTI.GetName(), MeshPrimitiveDto::MeshDtoConvertToPolicy));
    CommandBus::Post(std::make_shared<RegisterDtoPolicyConverter>(SkinMeshPrimitive::TYPE_RTTI.GetName(), SkinMeshPrimitiveDto::SkinMeshDtoConvertToPolicy));
}

MeshPrimitiveBuilder::~MeshPrimitiveBuilder()
{
    m_policy = nullptr;

    CommandBus::Post(std::make_shared<UnRegisterDtoPolicyConverter>(MeshPrimitive::TYPE_RTTI.GetName()));
    CommandBus::Post(std::make_shared<UnRegisterDtoPolicyConverter>(SkinMeshPrimitive::TYPE_RTTI.GetName()));

    EventPublisher::Unsubscribe(typeid(GeometryDataBuilt), m_onGeometryDataBuilt);
    m_onGeometryDataBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildGeometryDataFailed), m_onBuildGeometryDataFailed);
    m_onBuildGeometryDataFailed = nullptr;
    EventPublisher::Unsubscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    EventPublisher::Unsubscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onBuildRenderBufferFailed = nullptr;

    EventPublisher::Unsubscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    EventPublisher::Unsubscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
    m_onCompileEffectMaterialFailed = nullptr;

    EventPublisher::Unsubscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;
    EventPublisher::Unsubscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
    m_onLoadTextureFailed = nullptr;
    EventPublisher::Unsubscribe(typeid(TextureCreated), m_onTextureCreated);
    m_onTextureCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(CreateTextureFailed), m_onCreateTextureFailed);
    m_onCreateTextureFailed = nullptr;
}

void MeshPrimitiveBuilder::BuildMeshPrimitive(const Frameworks::Ruid& ruid, const std::shared_ptr<MeshPrimitivePolicy>& policy)
{
    m_buildingRuid = ruid;
    m_policy = policy;

    m_builtPrimitive = m_policy->CreatePrimitive();

    m_originalGeometryDesc = m_policy->GeometryFactoryDesc();
    m_builtGeometry = nullptr;
    m_builtRenderBuffer = nullptr;
    m_builtEffects.clear();
    m_builtTextures.clear();
    Frameworks::CommandBus::Post(std::make_shared<BuildGeometryData>(m_policy->GeometryPolicy()));
}

void MeshPrimitiveBuilder::OnGeometryDataBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GeometryDataBuilt, IEvent>(e);
    if (!ev) return;
    if ((m_policy) && (ev->GetName() != m_policy->GeometryPolicy().Name())) return;
    m_builtGeometry = ev->GetGeometryData();
    m_builtGeometry->TheFactoryDesc() = m_originalGeometryDesc;
    RenderBufferPolicy buffer;
    buffer.m_signature = m_builtGeometry->MakeRenderBufferSignature();
    buffer.m_renderBufferName = buffer.m_signature.GetName();
    buffer.m_vtxBufferName = buffer.m_renderBufferName + ".vtx";
    buffer.m_idxBufferName = buffer.m_renderBufferName + ".idx";
    buffer.m_sizeofVertex = m_builtGeometry->SizeofVertex();
    buffer.m_vtxBufferSize = m_builtGeometry->GetVertexCapacity() * m_builtGeometry->SizeofVertex();
    buffer.m_idxBufferSize = m_builtGeometry->GetIndexCapacity() * sizeof(unsigned);
    buffer.m_vtxBuffer = m_builtGeometry->GetVertexMemory();
    if (m_builtGeometry->GetIndexCapacity() > 0)
    {
        buffer.m_idxBuffer = m_builtGeometry->GetIndexMemory();
    }
    CommandBus::Post(std::make_shared<BuildRenderBuffer>(buffer));
}

void MeshPrimitiveBuilder::OnBuildGeometryDataFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildGeometryDataFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_policy->GeometryPolicy().Name()) return;
    EventPublisher::Post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingRuid, m_policy->Name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::OnRenderBufferBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_builtGeometry->MakeRenderBufferSignature().GetName()) return;
    m_builtRenderBuffer = ev->GetBuffer();
    std::dynamic_pointer_cast<MeshPrimitive, Primitive>(m_builtPrimitive)->LinkGeometryData(m_builtGeometry, m_builtRenderBuffer);
    m_builtEffects.resize(m_policy->EffectDtos().size());
    for (auto& dto : m_policy->EffectDtos())
    {
        CommandBus::Post(std::make_shared<CompileEffectMaterial>(dto));
    }
    m_builtTextures.resize(m_policy->TextureDtos().size());
    for (unsigned i = 0; i < m_policy->TextureDtos().size(); i++)
    {
        for (auto& t : m_policy->TextureDtos()[i].TextureMappings())
        {
            m_builtTextures[i].AppendTextureSemantic(t.Semantic());
            CommandBus::Post(std::make_shared<LoadTexture>(std::get<TexturePolicy>(t.ConvertToPolicy())));
        }
    }
}

void MeshPrimitiveBuilder::OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_builtGeometry->MakeRenderBufferSignature().GetName()) return;
    EventPublisher::Post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingRuid, m_policy->Name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::OnEffectMaterialCompiled(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialCompiled>(e);
    if (!ev) return;
    const std::optional<unsigned> found_idx = FindBuildingEffectIndex(ev->GetName());
    if (!found_idx) return;
    m_builtEffects[found_idx.value()] = ev->GetEffect();
    TryCompletingMesh();
}

void MeshPrimitiveBuilder::OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<CompileEffectMaterialFailed>(e);
    if (!ev) return;
    EventPublisher::Post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingRuid, m_policy->Name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::OnTextureLoadedOrCreated(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    std::string tex_name;
    std::shared_ptr<Texture> tex_loaded;
    if (const auto ev = std::dynamic_pointer_cast<TextureLoaded>(e))
    {
        tex_name = ev->GetName();
        tex_loaded = ev->GetTexture();
    }
    else if (const auto res = std::dynamic_pointer_cast<TextureCreated>(e))
    {
        tex_name = res->GetName();
        tex_loaded = res->GetTexture();
    }
    else
    {
        assert(false);
    }
    if (tex_name.empty()) return;
    const auto found_idx = FindLoadingTextureIndex(tex_name);
    if (!found_idx) return;

    const unsigned tex_idx = std::get<0>(found_idx.value());
    const unsigned tuple_idx = std::get<1>(found_idx.value());
    auto semantic = m_policy->TextureDtos()[tex_idx].TextureMappings()[tuple_idx].Semantic();
    auto array_idx = m_policy->TextureDtos()[tex_idx].TextureMappings()[tuple_idx].ArrayIndex();
    m_builtTextures[tex_idx].ChangeSemanticTexture({ semantic, tex_loaded, array_idx });
    TryCompletingMesh();
}

void MeshPrimitiveBuilder::OnLoadOrCreateTextureFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    std::error_code err;
    if (const auto ev = std::dynamic_pointer_cast<LoadTextureFailed>(e))
    {
        err = ev->GetErrorCode();
    }
    else if (const auto res = std::dynamic_pointer_cast<CreateTextureFailed>(e))
    {
        err = res->GetErrorCode();
    }
    else
    {
        assert(false);
    }
    if (err)
    {
        EventPublisher::Post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingRuid, m_policy->Name(), err));
    }
}

void MeshPrimitiveBuilder::TryCompletingMesh()
{
    if (!m_builtGeometry) return;
    if (!m_builtRenderBuffer) return;
    for (const auto& eff : m_builtEffects)
    {
        if (!eff) return;
    }
    for (auto& tex : m_builtTextures)
    {
        for (unsigned ti = 0; ti < tex.GetCount(); ti++)
        {
            if (tex.GetTexture(ti) == nullptr) return;
        }
    }
    m_builtPrimitive->ChangeEffectMaterial(m_builtEffects);
    m_builtPrimitive->ChangeTextureMaps(m_builtTextures);
    m_builtPrimitive->CreateRenderElements();
    m_builtPrimitive->RenderListID() = m_policy->RenderListId();
    m_builtPrimitive->SelectVisualTechnique(m_policy->VisualTechniqueSelection());
    EventPublisher::Post(std::make_shared<MeshPrimitiveBuilt>(m_buildingRuid, m_policy->Name(), m_builtPrimitive));
}

std::optional<unsigned> MeshPrimitiveBuilder::FindBuildingEffectIndex(const std::string& name)
{
    for (unsigned i = 0; i < m_policy->EffectDtos().size(); i++)
    {
        if ((m_policy->EffectDtos()[i].Name() == name) && (m_builtEffects[i] == nullptr)) return i;
    }
    return std::nullopt;
}

std::optional<std::tuple<unsigned, unsigned>> MeshPrimitiveBuilder::FindLoadingTextureIndex(const std::string& name)
{
    for (unsigned tex = 0; tex < m_policy->TextureDtos().size(); tex++)
    {
        for (unsigned tp = 0; tp < m_policy->TextureDtos()[tex].TextureMappings().size(); tp++)
        {
            if ((m_policy->TextureDtos()[tex].TextureMappings()[tp].TextureName() == name)
                && (m_builtTextures[tex].GetTexture(tp) == nullptr)) return std::make_tuple(tex, tp);
        }
    }
    return std::nullopt;
}

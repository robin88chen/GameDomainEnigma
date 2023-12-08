#include <memory>
#include "MeshPrimitiveBuilder.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ResponseBus.h"
#include "Geometries/GeometryCommands.h"
#include "Geometries/GeometryDataEvents.h"
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
using namespace Enigma::Geometries;

MeshPrimitiveBuilder::MeshPrimitiveBuilder() : m_originalGeometryDesc(GeometryData::TYPE_RTTI.getName()), m_buildingRuid()
{
    m_onGeometryDataBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnGeometryDataBuilt(e); });
    EventPublisher::subscribe(typeid(GeometryCreated), m_onGeometryDataBuilt);
    m_onBuildGeometryDataFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildGeometryDataFailed(e); });
    EventPublisher::subscribe(typeid(CreateGeometryFailed), m_onBuildGeometryDataFailed);
    m_onRenderBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnRenderBufferBuilt(e); });
    EventPublisher::subscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onBuildRenderBufferFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildRenderBufferFailed(e); });
    EventPublisher::subscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);

    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->OnEffectMaterialCompiled(e); });
    EventPublisher::subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnCompileEffectMaterialFailed(e); });
    EventPublisher::subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);

    m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoadedOrCreated(e); });
    EventPublisher::subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnLoadOrCreateTextureFailed(e); });
    EventPublisher::subscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
    m_onTextureCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoadedOrCreated(e); });
    EventPublisher::subscribe(typeid(TextureCreated), m_onTextureCreated);
    m_onCreateTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnLoadOrCreateTextureFailed(e); });
    EventPublisher::subscribe(typeid(CreateTextureFailed), m_onCreateTextureFailed);

    CommandBus::post(std::make_shared<RegisterDtoPolicyConverter>(MeshPrimitive::TYPE_RTTI.getName(), MeshPrimitiveDto::MeshDtoConvertToPolicy));
    CommandBus::post(std::make_shared<RegisterDtoPolicyConverter>(SkinMeshPrimitive::TYPE_RTTI.getName(), SkinMeshPrimitiveDto::SkinMeshDtoConvertToPolicy));
}

MeshPrimitiveBuilder::~MeshPrimitiveBuilder()
{
    m_policy = nullptr;

    CommandBus::post(std::make_shared<UnRegisterDtoPolicyConverter>(MeshPrimitive::TYPE_RTTI.getName()));
    CommandBus::post(std::make_shared<UnRegisterDtoPolicyConverter>(SkinMeshPrimitive::TYPE_RTTI.getName()));

    EventPublisher::unsubscribe(typeid(GeometryCreated), m_onGeometryDataBuilt);
    m_onGeometryDataBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(CreateGeometryFailed), m_onBuildGeometryDataFailed);
    m_onBuildGeometryDataFailed = nullptr;
    EventPublisher::unsubscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onBuildRenderBufferFailed = nullptr;

    EventPublisher::unsubscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onEffectMaterialCompiled = nullptr;
    EventPublisher::unsubscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);
    m_onCompileEffectMaterialFailed = nullptr;

    EventPublisher::unsubscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onTextureLoaded = nullptr;
    EventPublisher::unsubscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
    m_onLoadTextureFailed = nullptr;
    EventPublisher::unsubscribe(typeid(TextureCreated), m_onTextureCreated);
    m_onTextureCreated = nullptr;
    EventPublisher::unsubscribe(typeid(CreateTextureFailed), m_onCreateTextureFailed);
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
    Frameworks::CommandBus::post(std::make_shared<CreateGeometry>(m_policy->GeometryPolicy().id(), Rtti::fromName(m_policy->GeometryFactoryDesc().GetRttiName())));
}

void MeshPrimitiveBuilder::OnGeometryDataBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GeometryCreated, IEvent>(e);
    if (!ev) return;
    if ((m_policy) && (ev->id() != m_policy->GeometryPolicy().id())) return;
    m_builtGeometry = ev->geometryData();
    m_builtGeometry->factoryDesc() = m_originalGeometryDesc;
    RenderBufferPolicy buffer;
    buffer.m_signature = m_builtGeometry->makeRenderBufferSignature();
    buffer.m_renderBufferName = buffer.m_signature.getName();
    buffer.m_vtxBufferName = buffer.m_renderBufferName + ".vtx";
    buffer.m_idxBufferName = buffer.m_renderBufferName + ".idx";
    buffer.m_sizeofVertex = m_builtGeometry->sizeofVertex();
    buffer.m_vtxBufferSize = m_builtGeometry->getVertexCapacity() * m_builtGeometry->sizeofVertex();
    buffer.m_idxBufferSize = m_builtGeometry->getIndexCapacity() * sizeof(unsigned);
    buffer.m_vtxBuffer = m_builtGeometry->getVertexMemory();
    if (m_builtGeometry->getIndexCapacity() > 0)
    {
        buffer.m_idxBuffer = m_builtGeometry->getIndexMemory();
    }
    CommandBus::post(std::make_shared<BuildRenderBuffer>(buffer));
}

void MeshPrimitiveBuilder::OnBuildGeometryDataFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<CreateGeometryFailed, IEvent>(e);
    if (!ev) return;
    if (ev->id() != m_policy->GeometryPolicy().id()) return;
    EventPublisher::post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingRuid, m_policy->Name(), ev->error()));
}

void MeshPrimitiveBuilder::OnRenderBufferBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_builtGeometry->makeRenderBufferSignature().getName()) return;
    m_builtRenderBuffer = ev->GetBuffer();
    std::dynamic_pointer_cast<MeshPrimitive, Primitive>(m_builtPrimitive)->LinkGeometryData(m_builtGeometry, m_builtRenderBuffer);
    m_builtEffects.resize(m_policy->EffectDtos().size());
    for (auto& dto : m_policy->EffectDtos())
    {
        CommandBus::post(std::make_shared<CompileEffectMaterial>(dto));
    }
    m_builtTextures.resize(m_policy->TextureDtos().size());
    for (unsigned i = 0; i < m_policy->TextureDtos().size(); i++)
    {
        for (auto& t : m_policy->TextureDtos()[i].TextureMappings())
        {
            m_builtTextures[i].AppendTextureSemantic(t.Semantic());
            CommandBus::post(std::make_shared<LoadTexture>(std::get<TexturePolicy>(t.ConvertToPolicy())));
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
    if (ev->getName() != m_builtGeometry->makeRenderBufferSignature().getName()) return;
    EventPublisher::post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingRuid, m_policy->Name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::OnEffectMaterialCompiled(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialCompiled>(e);
    if (!ev) return;
    const std::optional<unsigned> found_idx = FindBuildingEffectIndex(ev->getName());
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
    EventPublisher::post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingRuid, m_policy->Name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::OnTextureLoadedOrCreated(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    std::string tex_name;
    std::shared_ptr<Texture> tex_loaded;
    if (const auto ev = std::dynamic_pointer_cast<TextureLoaded>(e))
    {
        tex_name = ev->getName();
        tex_loaded = ev->GetTexture();
    }
    else if (const auto res = std::dynamic_pointer_cast<TextureCreated>(e))
    {
        tex_name = res->getName();
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
        EventPublisher::post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingRuid, m_policy->Name(), err));
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
    EventPublisher::post(std::make_shared<MeshPrimitiveBuilt>(m_buildingRuid, m_policy->Name(), m_builtPrimitive));
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

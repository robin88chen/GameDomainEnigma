#include "MeshPrimitiveBuilder.h"
#include "MeshPrimitive.h"
#include "RenderablePrimitiveEvents.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/GeometryCommands.h"
#include "GameEngine/GeometryDataEvents.h"
#include "GameEngine/RenderBufferCommands.h"
#include "GameEngine/RenderBufferEvents.h"
#include "GameEngine/EffectCommands.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"

using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

MeshPrimitiveBuilder::MeshPrimitiveBuilder()
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
    m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->OnTextureLoaded(e); });
    EventPublisher::Subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnLoadTextureFailed(e); });
    EventPublisher::Subscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
}

MeshPrimitiveBuilder::~MeshPrimitiveBuilder()
{
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
}

void MeshPrimitiveBuilder::BuildMeshPrimitive(const MeshPrimitivePolicy& policy)
{
    m_policy = policy;
    m_builtPrimitive = std::make_shared<MeshPrimitive>(policy.Name());
    m_builtGeometry = nullptr;
    m_builtRenderBuffer = nullptr;
    m_builtEffects.clear();
    m_builtTextures.clear();
    Frameworks::CommandBus::Post(std::make_shared<BuildGeometryData>(policy.GeometryPolicy()));
}

void MeshPrimitiveBuilder::OnGeometryDataBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<GeometryDataBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_policy.GeometryPolicy().Name()) return;
    m_builtGeometry = ev->GetGeometryData();
    RenderBufferPolicy buffer;
    buffer.m_signature = m_builtGeometry->MakeRenderBufferSignature();
    buffer.m_renderBufferName = buffer.m_signature.GetName();
    buffer.m_vtxBufferName = buffer.m_renderBufferName + ".vtx";
    buffer.m_idxBufferName = buffer.m_renderBufferName + ".idx";
    buffer.m_sizeofVertex = m_builtGeometry->SizeofVertex();
    buffer.m_vtxBufferSize = m_builtGeometry->GetVertexCapacity() * m_builtGeometry->SizeofVertex();
    buffer.m_idxBufferSize = m_builtGeometry->GetIndexCapacity();
    buffer.m_vtxBuffer = m_builtGeometry->GetVertexMemory();
    if (m_builtGeometry->GetIndexCapacity() > 0)
    {
        buffer.m_idxBuffer = m_builtGeometry->GetIndexMemory();
    }
    CommandBus::Post(std::make_shared<BuildRenderBuffer>(buffer));
}

void MeshPrimitiveBuilder::OnBuildGeometryDataFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildGeometryDataFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_policy.GeometryPolicy().Name()) return;
    EventPublisher::Post(std::make_shared<BuildRenderablePrimitiveFailed>(m_policy.Name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::OnRenderBufferBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_builtGeometry) return;
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_policy.GeometryPolicy().Name()) return;
    m_builtRenderBuffer = ev->GetBuffer();
    std::dynamic_pointer_cast<MeshPrimitive, Primitive>(m_builtPrimitive)->LinkGeometryData(m_builtGeometry, m_builtRenderBuffer);
    m_builtEffects.resize(m_policy.EffectPolicies().size());
    for (auto& p : m_policy.EffectPolicies())
    {
        CommandBus::Post(std::make_shared<CompileEffectMaterial>(p));
    }
    m_builtTextures.resize(m_policy.TexturePolicies().size());
    for (unsigned i = 0; i < m_policy.TexturePolicies().size(); i++)
    {
        for (auto& t : m_policy.TexturePolicies()[i].GetTuplePolicies())
        {
            m_builtTextures[i].AppendTextureSemantic(std::get<std::string>(t));
            CommandBus::Post(std::make_shared<LoadTexture>(std::get<TexturePolicy>(t)));
        }
    }
}

void MeshPrimitiveBuilder::OnBuildRenderBufferFailed(const Frameworks::IEventPtr& e)
{
    if (!m_builtGeometry) return;
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_builtGeometry->MakeRenderBufferSignature().GetName()) return;
    EventPublisher::Post(std::make_shared<BuildRenderablePrimitiveFailed>(m_policy.Name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::OnEffectMaterialCompiled(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<EffectMaterialCompiled, IEvent>(e);
    if (!ev) return;
    std::optional<unsigned> found_idx = FindBuildingEffectIndex(ev->GetFilename());
    if (!found_idx) return;
    m_builtEffects[found_idx.value()] = ev->GetEffect();
    TryCompletingMesh();
}

void MeshPrimitiveBuilder::OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<CompileEffectMaterialFailed, IEvent>(e);
    if (!ev) return;
    std::optional<unsigned> found_idx = FindBuildingEffectIndex(ev->GetFilename());
    if (!found_idx) return;
    EventPublisher::Post(std::make_shared<BuildRenderablePrimitiveFailed>(m_policy.Name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::OnTextureLoaded(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<TextureLoaded, IEvent>(e);
    if (!ev) return;
    auto found_idx = FindLoadingTextureIndex(ev->GetTextureName());
    if (!found_idx) return;
    unsigned tex_idx = std::get<0>(found_idx.value());
    unsigned tuple_idx = std::get<1>(found_idx.value());
    m_builtTextures[tex_idx].ChangeTexture({ std::get<std::string>(m_policy.GetTextureTuplePolicy(tex_idx, tuple_idx)),
        ev->GetTexture(), std::get<std::optional<unsigned>>(m_policy.GetTextureTuplePolicy(tex_idx, tuple_idx)) });
    TryCompletingMesh();
}

void MeshPrimitiveBuilder::OnLoadTextureFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LoadTextureFailed, IEvent>(e);
    if (!ev) return;
    auto found_idx = FindLoadingTextureIndex(ev->GetTextureName());
    if (!found_idx) return;
    EventPublisher::Post(std::make_shared<BuildRenderablePrimitiveFailed>(m_policy.Name(), ev->GetError()));
}

void MeshPrimitiveBuilder::TryCompletingMesh()
{
    if (!m_builtGeometry) return;
    if (!m_builtRenderBuffer) return;
    for (auto& eff : m_builtEffects)
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
    m_builtPrimitive->ChangeTextureMap(m_builtTextures);
    m_builtPrimitive->CreateRenderElements();
    EventPublisher::Post(std::make_shared<RenderablePrimitiveBuilt>(m_policy.Name(), m_builtPrimitive));
}

std::optional<unsigned> MeshPrimitiveBuilder::FindBuildingEffectIndex(const std::string& name)
{
    for (unsigned i = 0; i < m_policy.EffectPolicies().size(); i++)
    {
        if ((m_policy.EffectPolicies()[i].Name() == name) && (m_builtEffects[i] == nullptr)) return i;
    }
    return std::nullopt;
}

std::optional<std::tuple<unsigned, unsigned>> MeshPrimitiveBuilder::FindLoadingTextureIndex(const std::string& name)
{
    for (unsigned tex = 0; tex < m_policy.TexturePolicies().size(); tex++)
    {
        for (unsigned tp = 0; tp < m_policy.TexturePolicies()[tex].GetTuplePolicies().size(); tp++)
        {
            if ((std::get<TexturePolicy>(m_policy.GetTextureTuplePolicy(tex, tp)).m_name == name)
                && (m_builtTextures[tex].GetTexture(tp) == nullptr)) return std::make_tuple(tex, tp);
        }
    }
    return std::nullopt;
}

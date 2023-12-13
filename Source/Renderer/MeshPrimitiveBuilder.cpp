#include <memory>
#include "MeshPrimitiveBuilder.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
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

MeshPrimitiveBuilder::MeshPrimitiveBuilder()
{
    m_onRenderBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onRenderBufferBuilt(e); });
    EventPublisher::subscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onBuildRenderBufferFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onBuildRenderBufferFailed(e); });
    EventPublisher::subscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);

    m_onEffectMaterialCompiled = std::make_shared<EventSubscriber>([=](auto e) { this->onEffectMaterialCompiled(e); });
    EventPublisher::subscribe(typeid(EffectMaterialCompiled), m_onEffectMaterialCompiled);
    m_onCompileEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onCompileEffectMaterialFailed(e); });
    EventPublisher::subscribe(typeid(CompileEffectMaterialFailed), m_onCompileEffectMaterialFailed);

    m_onTextureLoaded = std::make_shared<EventSubscriber>([=](auto e) { this->onTextureLoadedOrCreated(e); });
    EventPublisher::subscribe(typeid(TextureLoaded), m_onTextureLoaded);
    m_onLoadTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onLoadOrCreateTextureFailed(e); });
    EventPublisher::subscribe(typeid(LoadTextureFailed), m_onLoadTextureFailed);
    m_onTextureCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onTextureLoadedOrCreated(e); });
    EventPublisher::subscribe(typeid(TextureCreated), m_onTextureCreated);
    m_onCreateTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onLoadOrCreateTextureFailed(e); });
    EventPublisher::subscribe(typeid(CreateTextureFailed), m_onCreateTextureFailed);

    CommandBus::post(std::make_shared<RegisterDtoPolicyConverter>(SkinMeshPrimitive::TYPE_RTTI.getName(), SkinMeshPrimitiveDto::skinMeshDtoConvertToPolicy));
}

MeshPrimitiveBuilder::~MeshPrimitiveBuilder()
{
    CommandBus::post(std::make_shared<UnRegisterDtoPolicyConverter>(MeshPrimitive::TYPE_RTTI.getName()));
    CommandBus::post(std::make_shared<UnRegisterDtoPolicyConverter>(SkinMeshPrimitive::TYPE_RTTI.getName()));

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

void MeshPrimitiveBuilder::constituteLazyMeshPrimitive(const std::shared_ptr<MeshPrimitive>& mesh, const Engine::GenericDto& dto)
{
    m_buildingDto = MeshPrimitiveDto::fromGenericDto(dto);
    m_metaDto = std::make_unique<MeshPrimitiveMetaDto>(m_buildingDto.value());
    m_buildingId = mesh->id();
    m_builtPrimitive = mesh;
    m_builtGeometry = mesh->getGeometryData();
    m_builtPrimitive->lazyStatus().changeStatus(LazyStatus::Status::Loading);
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

void MeshPrimitiveBuilder::onRenderBufferBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_builtGeometry->makeRenderBufferSignature().getName()) return;
    m_builtRenderBuffer = ev->GetBuffer();
    std::dynamic_pointer_cast<MeshPrimitive, Primitive>(m_builtPrimitive)->linkGeometryData(m_builtGeometry, m_builtRenderBuffer);
    m_builtEffects.resize(m_metaDto->effects().size());
    for (auto& dto : m_metaDto->effects())
    {
        CommandBus::post(std::make_shared<CompileEffectMaterial>(dto));
    }
    m_builtTextures.resize(m_metaDto->textureMaps().size());
    for (unsigned i = 0; i < m_metaDto->textureMaps().size(); i++)
    {
        for (auto& t : m_metaDto->textureMaps()[i].TextureMappings())
        {
            m_builtTextures[i].AppendTextureSemantic(t.Semantic());
            CommandBus::post(std::make_shared<LoadTexture>(std::get<TexturePolicy>(t.ConvertToPolicy())));
        }
    }
}

void MeshPrimitiveBuilder::onBuildRenderBufferFailed(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_builtGeometry->makeRenderBufferSignature().getName()) return;
    EventPublisher::post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingId, m_buildingId.name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::onEffectMaterialCompiled(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialCompiled>(e);
    if (!ev) return;
    const std::optional<unsigned> found_idx = findBuildingEffectIndex(ev->getName());
    if (!found_idx) return;
    m_builtEffects[found_idx.value()] = ev->GetEffect();
    tryCompletingMesh();
}

void MeshPrimitiveBuilder::onCompileEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<CompileEffectMaterialFailed>(e);
    if (!ev) return;
    EventPublisher::post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingId, m_buildingId.name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::onTextureLoadedOrCreated(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
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
    const auto found_idx = findLoadingTextureIndex(tex_name);
    if (!found_idx) return;

    const unsigned tex_idx = std::get<0>(found_idx.value());
    const unsigned tuple_idx = std::get<1>(found_idx.value());
    auto semantic = m_metaDto->textureMaps()[tex_idx].TextureMappings()[tuple_idx].Semantic();
    auto array_idx = m_metaDto->textureMaps()[tex_idx].TextureMappings()[tuple_idx].ArrayIndex();
    m_builtTextures[tex_idx].changeSemanticTexture({ semantic, tex_loaded, array_idx });
    tryCompletingMesh();
}

void MeshPrimitiveBuilder::onLoadOrCreateTextureFailed(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
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
        EventPublisher::post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingId, m_buildingId.name(), err));
    }
}

void MeshPrimitiveBuilder::tryCompletingMesh()
{
    if (!m_buildingDto) return;
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
    m_builtPrimitive->changeEffectMaterials(m_builtEffects);
    m_builtPrimitive->changeTextureMaps(m_builtTextures);
    m_builtPrimitive->createRenderElements();
    m_builtPrimitive->renderListId() = m_buildingDto->renderListID();
    m_builtPrimitive->selectVisualTechnique(m_buildingDto->visualTechniqueSelection());
    m_builtPrimitive->lazyStatus().changeStatus(LazyStatus::Status::Ready);
    EventPublisher::post(std::make_shared<MeshPrimitiveBuilt>(m_buildingId, m_buildingId.name(), m_builtPrimitive));

    m_buildingDto = std::nullopt;
    m_metaDto = nullptr;
}

std::optional<unsigned> MeshPrimitiveBuilder::findBuildingEffectIndex(const std::string& name)
{
    assert(m_metaDto);
    for (unsigned i = 0; i < m_metaDto->effects().size(); i++)
    {
        if ((m_metaDto->effects()[i].Name() == name) && (m_builtEffects[i] == nullptr)) return i;
    }
    return std::nullopt;
}

std::optional<std::tuple<unsigned, unsigned>> MeshPrimitiveBuilder::findLoadingTextureIndex(const std::string& name)
{
    assert(m_metaDto);
    for (unsigned tex = 0; tex < m_metaDto->textureMaps().size(); tex++)
    {
        for (unsigned tp = 0; tp < m_metaDto->textureMaps()[tex].TextureMappings().size(); tp++)
        {
            if ((m_metaDto->textureMaps()[tex].TextureMappings()[tp].TextureName() == name)
                && (m_builtTextures[tex].GetTexture(tp) == nullptr)) return std::make_tuple(tex, tp);
        }
    }
    return std::nullopt;
}

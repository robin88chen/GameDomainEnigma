#include <memory>
#include "MeshPrimitiveBuilder.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"
#include "Geometries/GeometryCommands.h"
#include "Geometries/GeometryDataEvents.h"
#include "GameEngine/RenderBufferCommands.h"
#include "GameEngine/RenderBufferEvents.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/EffectQueries.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
#include "GameEngine/TextureQueries.h"
#include "GameEngine/Texture.h"
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

    m_onEffectMaterialContented = std::make_shared<EventSubscriber>([=](auto e) { this->onEffectMaterialContented(e); });
    EventPublisher::subscribe(typeid(EffectMaterialContented), m_onEffectMaterialContented);
    m_onContentEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onContentEffectMaterialFailed(e); });
    EventPublisher::subscribe(typeid(ContentEffectMaterialFailed), m_onContentEffectMaterialFailed);

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

    EventPublisher::unsubscribe(typeid(EffectMaterialContented), m_onEffectMaterialContented);
    m_onEffectMaterialContented = nullptr;
    EventPublisher::unsubscribe(typeid(ContentEffectMaterialFailed), m_onContentEffectMaterialFailed);
    m_onContentEffectMaterialFailed = nullptr;

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
    bool all_effect_ready = true;
    for (auto& id : m_metaDto->effects())
    {
        auto query = std::make_shared<QueryEffectMaterial>(id);
        QueryDispatcher::dispatch(query);
        if (auto eff = query->getResult())
        {
            m_builtEffects.push_back(eff);
            if (!eff->lazyStatus().isReady())
            {
                all_effect_ready = false;
            }
        }
    }
    m_builtTextures.resize(m_metaDto->textureMaps().size());
    bool all_texture_ready = true;
    for (unsigned i = 0; i < m_metaDto->textureMaps().size(); i++)
    {
        for (auto& t : m_metaDto->textureMaps()[i].textureMappings())
        {
            m_builtTextures[i].appendTextureSemantic(t.semantic());
            auto query = std::make_shared<QueryTexture>(t.textureId());
            QueryDispatcher::dispatch(query);
            if (auto tex = query->getResult())
            {
                m_builtTextures[i].changeSemanticTexture({ t.semantic(), tex, t.arrayIndex() });
                if (!tex->lazyStatus().isReady())
                {
                    all_texture_ready = false;
                }
            }
        }
    }
    if ((all_effect_ready) && (all_texture_ready))
    {
        tryCompletingMesh();
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

void MeshPrimitiveBuilder::onEffectMaterialContented(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialContented>(e);
    if (!ev) return;
    const std::optional<unsigned> found_idx = findBuildingEffectIndex(ev->sourceId());
    if (!found_idx) return;
    if (m_builtEffects[found_idx.value()]->lazyStatus().isReady())
    {
        tryCompletingMesh();
    }
}

void MeshPrimitiveBuilder::onContentEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<ContentEffectMaterialFailed>(e);
    if (!ev) return;
    EventPublisher::post(std::make_shared<BuildMeshPrimitiveFailed>(m_buildingId, m_buildingId.name(), ev->error()));
}

void MeshPrimitiveBuilder::onTextureLoadedOrCreated(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!e) return;
    TextureId tex_id;

    if (const auto ev = std::dynamic_pointer_cast<TextureLoaded>(e))
    {
        tex_id = ev->id();
    }
    else if (const auto res = std::dynamic_pointer_cast<TextureCreated>(e))
    {
        tex_id = res->id();
    }
    else
    {
        assert(false);
    }
    const auto found_idx = findLoadingTextureIndex(tex_id);
    if (!found_idx) return;

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
        err = ev->error();
    }
    else if (const auto res = std::dynamic_pointer_cast<CreateTextureFailed>(e))
    {
        err = res->error();
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
        for (unsigned ti = 0; ti < tex.getCount(); ti++)
        {
            if (tex.getTexture(ti) == nullptr) return;
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

std::optional<unsigned> MeshPrimitiveBuilder::findBuildingEffectIndex(const EffectMaterialId& id)
{
    assert(m_metaDto);
    for (unsigned i = 0; i < m_metaDto->effects().size(); i++)
    {
        if ((m_metaDto->effects()[i] == id) && (m_builtEffects[i] != nullptr)) return i;
    }
    return std::nullopt;
}

std::optional<std::tuple<unsigned, unsigned>> MeshPrimitiveBuilder::findLoadingTextureIndex(const TextureId& id)
{
    assert(m_metaDto);
    for (unsigned tex = 0; tex < m_metaDto->textureMaps().size(); tex++)
    {
        for (unsigned tp = 0; tp < m_metaDto->textureMaps()[tex].textureMappings().size(); tp++)
        {
            if ((m_metaDto->textureMaps()[tex].textureMappings()[tp].textureId() == id)
                && (m_builtTextures[tex].getTexture(tp) == nullptr)) return std::make_tuple(tex, tp);
        }
    }
    return std::nullopt;
}

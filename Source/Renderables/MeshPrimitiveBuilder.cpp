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

using namespace Enigma::Renderables;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Geometries;

MeshPrimitiveBuilder::MeshPrimitiveBuilder()
{
    m_onRenderBufferBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onRenderBufferBuilt(e); });
    EventPublisher::subscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onBuildRenderBufferFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onBuildRenderBufferFailed(e); });
    EventPublisher::subscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);

    m_onEffectMaterialHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onEffectMaterialContented(e); });
    EventPublisher::subscribe(typeid(EffectMaterialHydrated), m_onEffectMaterialHydrated);
    m_onHydrateEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onContentEffectMaterialFailed(e); });
    EventPublisher::subscribe(typeid(HydrateEffectMaterialFailed), m_onHydrateEffectMaterialFailed);

    m_onTextureHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onTextureContented(e); });
    EventPublisher::subscribe(typeid(TextureHydrated), m_onTextureHydrated);
    m_onHydrateTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onContentTextureFailed(e); });
    EventPublisher::subscribe(typeid(HydrateTextureFailed), m_onHydrateTextureFailed);
}

MeshPrimitiveBuilder::~MeshPrimitiveBuilder()
{
    CommandBus::post(std::make_shared<UnRegisterDtoPolicyConverter>(MeshPrimitive::TYPE_RTTI.getName()));
    CommandBus::post(std::make_shared<UnRegisterDtoPolicyConverter>(SkinMeshPrimitive::TYPE_RTTI.getName()));

    EventPublisher::unsubscribe(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onBuildRenderBufferFailed = nullptr;

    EventPublisher::unsubscribe(typeid(EffectMaterialHydrated), m_onEffectMaterialHydrated);
    m_onEffectMaterialHydrated = nullptr;
    EventPublisher::unsubscribe(typeid(HydrateEffectMaterialFailed), m_onHydrateEffectMaterialFailed);
    m_onHydrateEffectMaterialFailed = nullptr;

    EventPublisher::unsubscribe(typeid(TextureHydrated), m_onTextureHydrated);
    m_onTextureHydrated = nullptr;
    EventPublisher::unsubscribe(typeid(HydrateTextureFailed), m_onHydrateTextureFailed);
    m_onHydrateTextureFailed = nullptr;
}

void MeshPrimitiveBuilder::hydrateMeshPrimitive(const std::shared_ptr<MeshPrimitive>& mesh, const Engine::GenericDto& dto)
{
    m_buildingDto = MeshPrimitiveDto(dto);
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
    std::dynamic_pointer_cast<MeshPrimitive>(m_builtPrimitive)->linkGeometryData(m_builtGeometry, m_builtRenderBuffer);
    bool all_effect_ready = true;
    for (auto& id : m_metaDto->effects())
    {
        if (auto eff = EffectMaterial::queryEffectMaterial(id))
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
            if (auto tex = Texture::queryTexture(t.textureId()))
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
    EventPublisher::post(std::make_shared<HydrateMeshPrimitiveFailed>(m_buildingId, m_buildingId.name(), ev->GetErrorCode()));
}

void MeshPrimitiveBuilder::onEffectMaterialContented(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialHydrated>(e);
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
    const auto ev = std::dynamic_pointer_cast<HydrateEffectMaterialFailed>(e);
    if (!ev) return;
    EventPublisher::post(std::make_shared<HydrateMeshPrimitiveFailed>(m_buildingId, m_buildingId.name(), ev->error()));
}

void MeshPrimitiveBuilder::onTextureContented(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureHydrated>(e);
    if (!ev) return;
    const auto found_idx = findLoadingTextureIndex(ev->id());
    if (!found_idx) return;

    tryCompletingMesh();
}

void MeshPrimitiveBuilder::onContentTextureFailed(const Frameworks::IEventPtr& e)
{
    if (!m_metaDto) return;
    if (!m_buildingDto) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<HydrateTextureFailed>(e);
    if (!ev) return;
    if (ev->error())
    {
        EventPublisher::post(std::make_shared<HydrateMeshPrimitiveFailed>(m_buildingId, m_buildingId.name(), ev->error()));
    }
}

void MeshPrimitiveBuilder::tryCompletingMesh()
{
    if (!m_buildingDto) return;
    if (!m_builtGeometry) return;
    if (!m_builtRenderBuffer) return;
    for (const auto& eff : m_builtEffects)
    {
        if ((!eff) || (!eff->lazyStatus().isReady())) return;
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
    EventPublisher::post(std::make_shared<MeshPrimitiveHydrated>(m_buildingId, m_buildingId.name(), m_builtPrimitive));

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
                && (m_builtTextures.size() > tex) && (m_builtTextures[tex].getTexture(tp))
                && (m_builtTextures[tex].getTexture(tp)->id() == id) && (m_builtTextures[tex].getTexture(tp)->lazyStatus().isReady()))
                return std::make_tuple(tex, tp);
        }
    }
    return std::nullopt;
}

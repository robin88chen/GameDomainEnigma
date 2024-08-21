#include <memory>
#include "MeshPrimitiveBuilder.h"
#include "MeshPrimitive.h"
#include "MeshPrimitiveAssembler.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/RenderBufferCommands.h"
#include "GameEngine/RenderBufferEvents.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
#include "GameEngine/Texture.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "GameEngine/TextureMappingAssembler.h"
#include "Platforms/PlatformLayer.h"

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

    m_onEffectMaterialHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onEffectMaterialHydrated(e); });
    EventPublisher::subscribe(typeid(EffectMaterialHydrated), m_onEffectMaterialHydrated);
    m_onHydrateEffectMaterialFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onHydrateEffectMaterialFailed(e); });
    EventPublisher::subscribe(typeid(HydrateEffectMaterialFailed), m_onHydrateEffectMaterialFailed);

    m_onTextureHydrated = std::make_shared<EventSubscriber>([=](auto e) { this->onTextureHydrated(e); });
    EventPublisher::subscribe(typeid(TextureHydrated), m_onTextureHydrated);
    m_onHydrateTextureFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onHydrateTextureFailed(e); });
    EventPublisher::subscribe(typeid(HydrateTextureFailed), m_onHydrateTextureFailed);
}

MeshPrimitiveBuilder::~MeshPrimitiveBuilder()
{
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
    Platforms::Debug::Printf("hydrating mesh primitive %s\n", mesh->id().name().c_str());
    cleanupBuildingMeta();
    m_buildingDisassembler = std::make_shared<MeshPrimitiveDisassembler>();
    m_buildingDisassembler->disassemble(dto);
    m_metaDisassembler = std::make_unique<MeshPrimitiveMaterialMetaDisassembler>(m_buildingDisassembler);
    m_metaDisassembler->replaceDuplicatedEffects(mesh);
    m_builtEffects.clear();
    m_builtTextures.clear();
    m_buildingId = mesh->id();
    m_builtPrimitive = mesh;
    m_builtGeometry = mesh->getGeometryData();
    m_builtPrimitive->lazyStatus().changeStatus(LazyStatus::Status::Loading);
    RenderBufferPolicy buffer;
    buffer.m_signature = m_builtGeometry->makeRenderBufferSignature();
    buffer.m_renderBufferName = buffer.m_signature.name();
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
    CommandBus::enqueue(std::make_shared<BuildRenderBuffer>(buffer));
}

void MeshPrimitiveBuilder::onRenderBufferBuilt(const Frameworks::IEventPtr& e)
{
    Platforms::Debug::Printf("on render buffer built of mesh %s\n", m_buildingId.name().c_str());
    if (!m_metaDisassembler) return;
    if (!m_buildingDisassembler) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->name() != m_builtGeometry->makeRenderBufferSignature().name()) return;
    m_builtRenderBuffer = ev->buffer();
    std::dynamic_pointer_cast<MeshPrimitive>(m_builtPrimitive)->linkGeometryData(m_builtGeometry, m_builtRenderBuffer);
    bool all_effect_ready = true;
    for (auto& id : m_metaDisassembler->effects())
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
    m_builtTextures.resize(m_metaDisassembler->textureMaps().size());
    bool all_texture_ready = true;
    for (unsigned i = 0; i < m_metaDisassembler->textureMaps().size(); i++)
    {
        for (auto& t : m_metaDisassembler->textureMaps()[i]->textureMappings())
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
    Platforms::Debug::Printf("on render buffer build failure of mesh %s\n", m_buildingId.name().c_str());
    if (!m_metaDisassembler) return;
    if (!m_buildingDisassembler) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, IEvent>(e);
    if (!ev) return;
    if (ev->name() != m_builtGeometry->makeRenderBufferSignature().name()) return;
    failMeshHydration(ev->error());
}

void MeshPrimitiveBuilder::onEffectMaterialHydrated(const Frameworks::IEventPtr& e)
{
    //? 發生奇怪的bug, 兩個 dto 是 empty, 卻沒有return, 一路走到 found_idx 出錯, ev的 id 與 builtEffects 也不符
    Platforms::Debug::Printf("on effect material hydrated of mesh %s\n", m_buildingId.name().c_str());
    if (!m_metaDisassembler) return;
    if (!m_buildingDisassembler) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<EffectMaterialHydrated>(e);
    if (!ev) return;
    Platforms::Debug::Printf("on effect material hydrated of mesh %s, and material %s\n", m_buildingId.name().c_str(), ev->id().name().c_str());
    //todo 真正原因: render buffer 比 material 晚完成，所以這時候 builtEffects 還是空的，index 就出錯了
    const std::optional<unsigned> found_idx = findBuildingEffectIndex(ev->id());
    if (!found_idx) return;
    if (m_builtEffects[found_idx.value()]->lazyStatus().isReady())
    {
        tryCompletingMesh();
    }
}

void MeshPrimitiveBuilder::onHydrateEffectMaterialFailed(const Frameworks::IEventPtr& e)
{
    Platforms::Debug::Printf("on effect material hydrated failure of mesh %s\n", m_buildingId.name().c_str());
    if (!m_metaDisassembler) return;
    if (!m_buildingDisassembler) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<HydrateEffectMaterialFailed>(e);
    if (!ev) return;
    failMeshHydration(ev->error());
}

void MeshPrimitiveBuilder::onTextureHydrated(const Frameworks::IEventPtr& e)
{
    Platforms::Debug::Printf("on texture hydrated of mesh %s\n", m_buildingId.name().c_str());
    if (!m_metaDisassembler) return;
    if (!m_buildingDisassembler) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<TextureHydrated>(e);
    if (!ev) return;
    Platforms::Debug::Printf("on texture hydrated of mesh %s, and texture %s\n", m_buildingId.name().c_str(), ev->id().name().c_str());
    const auto found_idx = findLoadingTextureIndex(ev->id());
    if (!found_idx) return;

    tryCompletingMesh();
}

void MeshPrimitiveBuilder::onHydrateTextureFailed(const Frameworks::IEventPtr& e)
{
    Platforms::Debug::Printf("on texture hydrated failure of mesh %s\n", m_buildingId.name().c_str());
    if (!m_metaDisassembler) return;
    if (!m_buildingDisassembler) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<HydrateTextureFailed>(e);
    if (!ev) return;
    if (ev->error())
    {
        failMeshHydration(ev->error());
    }
}

void MeshPrimitiveBuilder::tryCompletingMesh()
{
    if (!m_buildingDisassembler) return;
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
    m_builtPrimitive->renderListId() = m_buildingDisassembler->renderListID();
    m_builtPrimitive->selectVisualTechnique(m_buildingDisassembler->visualTechniqueSelection());
    m_builtPrimitive->lazyStatus().changeStatus(LazyStatus::Status::Ready);
    EventPublisher::enqueue(std::make_shared<MeshPrimitiveHydrated>(m_buildingId, m_buildingId.name(), m_builtPrimitive));

    Platforms::Debug::Printf("mesh primitive %s build completed\n", m_buildingId.name().c_str());
    cleanupBuildingMeta();
}

void MeshPrimitiveBuilder::failMeshHydration(const std::error_code er)
{
    Platforms::Debug::ErrorPrintf("mesh primitive %s build failed : %s\n", m_buildingId.name().c_str(), er.message().c_str());
    EventPublisher::enqueue(std::make_shared<HydrateMeshPrimitiveFailed>(m_buildingId, m_buildingId.name(), er));
    cleanupBuildingMeta();
}

std::optional<unsigned> MeshPrimitiveBuilder::findBuildingEffectIndex(const EffectMaterialId& id)
{
    assert(m_metaDisassembler);
    for (unsigned i = 0; i < m_metaDisassembler->effects().size(); i++)
    {
        if ((m_metaDisassembler->effects()[i] == id) && (m_builtEffects.size() > i) && (m_builtEffects[i] != nullptr)) return i;
    }
    return std::nullopt;
}

std::optional<std::tuple<unsigned, unsigned>> MeshPrimitiveBuilder::findLoadingTextureIndex(const TextureId& id)
{
    assert(m_metaDisassembler);
    for (unsigned tex = 0; tex < m_metaDisassembler->textureMaps().size(); tex++)
    {
        for (unsigned tp = 0; tp < m_metaDisassembler->textureMaps()[tex]->textureMappings().size(); tp++)
        {
            if ((m_metaDisassembler->textureMaps()[tex]->textureMappings()[tp].textureId() == id)
                && (m_builtTextures.size() > tex) && (m_builtTextures[tex].getTexture(tp))
                && (m_builtTextures[tex].getTexture(tp)->id() == id) && (m_builtTextures[tex].getTexture(tp)->lazyStatus().isReady()))
                return std::make_tuple(tex, tp);
        }
    }
    return std::nullopt;
}

void MeshPrimitiveBuilder::cleanupBuildingMeta()
{
    m_builtRenderBuffer = nullptr;
    m_builtGeometry = nullptr;
    m_builtPrimitive = nullptr;
    m_buildingDisassembler = nullptr;
    m_metaDisassembler = nullptr;
    m_builtEffects.clear();
    m_builtTextures.clear();
    m_buildingId = Primitives::PrimitiveId();
}

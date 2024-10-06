#include <memory>
#include "MeshPrimitiveBuilder.h"
#include "MeshPrimitive.h"
#include "MeshPrimitiveAssembler.h"
#include "RenderableEvents.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/RenderBufferCommands.h"
#include "GameEngine/RenderBufferEvents.h"
#include "GameEngine/EffectEvents.h"
#include "GameEngine/TextureCommands.h"
#include "GameEngine/TextureEvents.h"
#include "GameEngine/Texture.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Renderables;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Geometries;

MeshPrimitiveBuilder::MeshPrimitiveBuilder()
{
    m_onRenderBufferBuilt = eventSubscription(typeid(RenderBufferBuilt), [=](auto e) { this->onRenderBufferBuilt(e); });
    m_onBuildRenderBufferFailed = eventSubscription(typeid(BuildRenderBufferFailed), [=](auto e) { this->onBuildRenderBufferFailed(e); });
    m_onPrimitiveMaterialHydrated = eventSubscription(typeid(PrimitiveMaterialHydrated), [=](auto e) { this->onPrimitiveMaterialHydrated(e); });
    m_onPrimitiveMaterialHydrationFailed = eventSubscription(typeid(PrimitiveMaterialHydrationFailed), [=](auto e) { this->onPrimitiveMaterialHydrationFailed(e); });
}

MeshPrimitiveBuilder::~MeshPrimitiveBuilder()
{
    releaseSubscription(typeid(RenderBufferBuilt), m_onRenderBufferBuilt);
    m_onRenderBufferBuilt = nullptr;
    releaseSubscription(typeid(BuildRenderBufferFailed), m_onBuildRenderBufferFailed);
    m_onBuildRenderBufferFailed = nullptr;
    releaseSubscription(typeid(PrimitiveMaterialHydrated), m_onPrimitiveMaterialHydrated);
    m_onPrimitiveMaterialHydrated = nullptr;
    releaseSubscription(typeid(PrimitiveMaterialHydrationFailed), m_onPrimitiveMaterialHydrationFailed);
    m_onPrimitiveMaterialHydrationFailed = nullptr;
}

void MeshPrimitiveBuilder::hydrateMeshPrimitive(const std::shared_ptr<MeshPrimitive>& mesh, const Engine::GenericDto& dto)
{
    Platforms::Debug::Printf("hydrating mesh primitive %s\n", mesh->id().name().c_str());
    cleanupBuildingMeta();
    m_buildingDisassembler = std::make_shared<MeshPrimitiveDisassembler>();
    m_buildingDisassembler->disassemble(dto);
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
    //if (!m_metaDisassembler) return;
    if (!m_buildingDisassembler) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderBufferBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->name() != m_builtGeometry->makeRenderBufferSignature().name()) return;
    m_builtRenderBuffer = ev->buffer();
    std::dynamic_pointer_cast<MeshPrimitive>(m_builtPrimitive)->linkGeometryData(m_builtGeometry, m_builtRenderBuffer);
    bool all_material_ready = true;
    for (auto& mat : m_buildingDisassembler->materials())
    {
        if (!mat->lazyStatus().isReady())
        {
            all_material_ready = false;
        }
    }
    if (all_material_ready)
    {
        tryCompletingMesh();
    }
}

void MeshPrimitiveBuilder::onBuildRenderBufferFailed(const Frameworks::IEventPtr& e)
{
    Platforms::Debug::Printf("on render buffer build failure of mesh %s\n", m_buildingId.name().c_str());
    if (!m_buildingDisassembler) return;
    if (!m_builtGeometry) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderBufferFailed, IEvent>(e);
    if (!ev) return;
    if (ev->name() != m_builtGeometry->makeRenderBufferSignature().name()) return;
    failMeshHydration(ev->error());
}

void MeshPrimitiveBuilder::onPrimitiveMaterialHydrated(const Frameworks::IEventPtr& e)
{
    //!? 紀錄曾經出現的問題
    //!? 發生奇怪的bug, 兩個 dto 是 empty, 卻沒有return, 一路走到 found_idx 出錯, ev的 id 與 builtEffects 也不符
    //!? 真正原因: render buffer 比 material 晚完成，所以這時候 builtEffects 還是空的，index 就出錯了
    Platforms::Debug::Printf("on material hydrated of mesh %s\n", m_buildingId.name().c_str());
    if (!m_buildingDisassembler) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimitiveMaterialHydrated>(e);
    if (!ev) return;
    tryCompletingMesh();
}

void MeshPrimitiveBuilder::onPrimitiveMaterialHydrationFailed(const Frameworks::IEventPtr& e)
{
    Platforms::Debug::Printf("on material hydrated failure of mesh %s\n", m_buildingId.name().c_str());
    if (!m_buildingDisassembler) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PrimitiveMaterialHydrationFailed>(e);
    if (!ev) return;
    if (!isBuildingMeshMaterial(ev->material())) return;
    failMeshHydration(ev->error());
}

void MeshPrimitiveBuilder::tryCompletingMesh()
{
    if (!m_buildingDisassembler) return;
    if (!m_builtGeometry) return;
    if (!m_builtRenderBuffer) return;
    for (auto& mat : m_buildingDisassembler->materials())
    {
        if (!mat->lazyStatus().isReady()) return;
    }
    m_builtPrimitive->changeMaterials(m_buildingDisassembler->materials());
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

bool MeshPrimitiveBuilder::isBuildingMeshMaterial(const std::shared_ptr<PrimitiveMaterial>& material) const
{
    if (!m_buildingDisassembler) return false;
    for (auto& mat : m_buildingDisassembler->materials())
    {
        if (mat == material) return true;
    }
    return false;
}

void MeshPrimitiveBuilder::cleanupBuildingMeta()
{
    m_builtRenderBuffer = nullptr;
    m_builtGeometry = nullptr;
    m_builtPrimitive = nullptr;
    m_buildingDisassembler = nullptr;
    m_buildingId = Primitives::PrimitiveId();
}

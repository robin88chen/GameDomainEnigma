#include "ModelPrimitiveBuilder.h"
#include "MeshPrimitiveBuilder.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "RenderablePrimitiveDtos.h"
#include "Frameworks/EventPublisher.h"
#include "RenderablePrimitiveEvents.h"

using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;

ModelPrimitiveBuilder::ModelPrimitiveBuilder()
{
    m_meshBuilder = menew MeshPrimitiveBuilder();
    m_onMeshPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnMeshPrimitiveBuilt(e); });
    m_onBuildMeshPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildMeshPrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onMeshPrimitiveBuilt);
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildMeshPrimitiveFailed);
}

ModelPrimitiveBuilder::~ModelPrimitiveBuilder()
{
    EventPublisher::Unsubscribe(typeid(RenderablePrimitiveBuilt), m_onMeshPrimitiveBuilt);
    EventPublisher::Unsubscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    m_onMeshPrimitiveBuilt = nullptr;
    m_onBuildMeshPrimitiveFailed = nullptr;

    SAFE_DELETE(m_meshBuilder);
}

void ModelPrimitiveBuilder::BuildModelPrimitive(const std::shared_ptr<ModelPrimitivePolicy>& policy)
{
    while (!m_meshPolicies.empty())
    {
        m_meshPolicies.pop();
    }
    m_meshBuildingMetas.clear();

    m_policy = policy;
    m_builtPrimitive = std::make_shared<ModelPrimitive>(m_policy->Name());
    for (auto& dto : m_policy->NodeTreeDto().MeshNodes())
    {
        MeshNodeDto node_dto = MeshNodeDto::FromGenericDto(dto);
        MeshNode node(node_dto.Name());
        node.SetLocalTransform(node_dto.LocalTransform());
        node.SetRootRefTransform(node_dto.RootRefTransform());
        if (node_dto.ParentIndexInArray())
        {
            node.SetParentIndexInArray(node_dto.ParentIndexInArray().value());
        }
        if (node_dto.TheMeshPrimitive())
        {
            PushInnerMesh(node_dto.Name(), MeshPrimitiveDto::FromGenericDto(node_dto.TheMeshPrimitive().value()).
                ConvertToPolicy(m_policy->TheDtoDeserializer(), m_policy->TheEffectDeserializer()));
        }
        m_builtPrimitive->GetMeshNodeTree().AddMeshNode(node);
    }
    ContinueBuildInnerMesh();
}

void ModelPrimitiveBuilder::PushInnerMesh(const std::string& node_name, const std::shared_ptr<MeshPrimitivePolicy>& policy)
{
    m_meshBuildingMetas.emplace_back(node_name, policy->GetRuid());
    m_meshPolicies.push(policy);
}

void ModelPrimitiveBuilder::ContinueBuildInnerMesh()
{
    if (m_meshPolicies.empty())
    {
        CompleteModelPrimitive();
        return;
    }
    m_meshBuilder->BuildMeshPrimitive(m_meshPolicies.front());
    m_meshPolicies.pop();
}

void ModelPrimitiveBuilder::CompleteModelPrimitive()
{
    if (!m_policy) return;
    EventPublisher::Post(std::make_shared<RenderablePrimitiveBuilt>(m_policy->GetRuid(), m_policy->Name(), m_builtPrimitive));
}

void ModelPrimitiveBuilder::OnMeshPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    auto mesh_prim = std::dynamic_pointer_cast<MeshPrimitive, Engine::Primitive>(ev->GetPrimitive());
    if (!mesh_prim) return;
    bool has_mesh_built = false;
    for (auto& meta : m_meshBuildingMetas)
    {
        if (meta.m_ruidPolicy == ev->GetRuid())
        {
            auto index = m_builtPrimitive->GetMeshNodeTree().FindMeshNodeIndex(meta.m_nodeName);
            if (index)
            {
                m_builtPrimitive->GetMeshNodeTree().GetMeshNode(index.value()).value().get().SetMeshPrimitive(mesh_prim);
                has_mesh_built = true;
            }
            break;
        }
    }
    if (has_mesh_built) ContinueBuildInnerMesh();
}

void ModelPrimitiveBuilder::OnBuildMeshPrimitiveFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderablePrimitiveFailed, IEvent>(e);
    if (!ev) return;
    for (auto& meta : m_meshBuildingMetas)
    {
        if (meta.m_ruidPolicy == ev->GetRuid())
        {
            EventPublisher::Post(std::make_shared<BuildRenderablePrimitiveFailed>(m_policy->GetRuid(), m_policy->Name(), ev->GetErrorCode()));
            return;
        }
    }
}

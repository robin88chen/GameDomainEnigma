#include "ModelPrimitiveBuilder.h"
#include "MeshPrimitiveBuilder.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Platforms/MemoryMacro.h"
#include "RenderablePrimitiveDtos.h"
#include "Frameworks/EventPublisher.h"
#include "SkinMeshPrimitive.h"
#include "Frameworks/CommandBus.h"
#include "Animators/AnimatorCommands.h"
#include "Animators/AnimatorEvents.h"
#include "Animators/ModelPrimitiveAnimator.h"

using namespace Enigma::Renderer;
using namespace Enigma::Frameworks;

ModelPrimitiveBuilder::ModelPrimitiveBuilder() : m_buildingRuid()
{
    m_meshBuilder = menew MeshPrimitiveBuilder();
    m_onMeshPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnMeshPrimitiveBuilt(e); });
    m_onBuildMeshPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildMeshPrimitiveFailed(e); });
    m_onModelAnimatorBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnModelAnimatorBuilt(e); });
    m_onBuildModelAnimatorFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildModelAnimatorFailed(e); });
    EventPublisher::Subscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveBuilt), m_onMeshPrimitiveBuilt);
    EventPublisher::Subscribe(typeid(MeshPrimitiveBuilder::BuildMeshPrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    EventPublisher::Subscribe(typeid(Animators::ModelAnimatorBuilt), m_onModelAnimatorBuilt);
    EventPublisher::Subscribe(typeid(Animators::BuildModelAnimatorFailed), m_onBuildModelAnimatorFailed);
}

ModelPrimitiveBuilder::~ModelPrimitiveBuilder()
{
    EventPublisher::Unsubscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveBuilt), m_onMeshPrimitiveBuilt);
    EventPublisher::Unsubscribe(typeid(MeshPrimitiveBuilder::BuildMeshPrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    EventPublisher::Unsubscribe(typeid(Animators::ModelAnimatorBuilt), m_onModelAnimatorBuilt);
    EventPublisher::Unsubscribe(typeid(Animators::BuildModelAnimatorFailed), m_onBuildModelAnimatorFailed);
    m_onMeshPrimitiveBuilt = nullptr;
    m_onBuildMeshPrimitiveFailed = nullptr;
    m_onModelAnimatorBuilt = nullptr;
    m_onBuildModelAnimatorFailed = nullptr;

    SAFE_DELETE(m_meshBuilder);
}

void ModelPrimitiveBuilder::BuildModelPrimitive(const Frameworks::Ruid& ruid, const std::shared_ptr<ModelPrimitivePolicy>& policy)
{
    while (!m_meshPolicies.empty())
    {
        m_meshPolicies.pop();
    }
    m_meshBuildingMetas.clear();

    m_buildingRuid = ruid;
    m_policy = policy;
    m_builtPrimitive = std::make_shared<ModelPrimitive>(m_policy->Name());
    for (auto& dto : m_policy->NodeTreeDto().MeshNodes())
    {
        MeshNodeDto node_dto = MeshNodeDto::FromGenericDto(dto);
        MeshNode node(node_dto.Name());
        node.SetT_PosTransform(node_dto.LocalT_PosTransform());
        node.SetLocalTransform(node_dto.LocalT_PosTransform());
        //node.SetRootRefTransform(node_dto.RootRefTransform());
        if (auto v = node_dto.ParentIndexInArray())
        {
            node.SetParentIndexInArray(v.value());
        }
        if (auto prim = node_dto.TheMeshPrimitive())
        {
            PushInnerMesh(node_dto.Name(), std::dynamic_pointer_cast<MeshPrimitivePolicy, Engine::GenericPolicy>(prim->ConvertToPolicy(m_policy->TheDtoDeserializer())));
        }
        m_builtPrimitive->GetMeshNodeTree().AddMeshNode(node);
    }
    ContinueBuildInnerMesh();
}

void ModelPrimitiveBuilder::PushInnerMesh(const std::string& node_name, const std::shared_ptr<MeshPrimitivePolicy>& policy)
{
    Ruid mesh_ruid = Ruid::Generate();
    m_meshBuildingMetas.emplace_back(node_name, mesh_ruid);
    m_meshPolicies.push({ mesh_ruid, policy });
}

void ModelPrimitiveBuilder::ContinueBuildInnerMesh()
{
    if (m_meshPolicies.empty())
    {
        TryBuildAnimator();
        return;
    }
    m_meshBuilder->BuildMeshPrimitive(std::get<Ruid>(m_meshPolicies.front()),
        std::get<std::shared_ptr<MeshPrimitivePolicy>>(m_meshPolicies.front()));
    m_meshPolicies.pop();
}

void ModelPrimitiveBuilder::TryBuildAnimator()
{
    if (!m_policy) return;
    if (auto ani = m_policy->TheModelAnimator())
    {
        m_animatorPolicy = ani->ConvertToPolicy(m_builtPrimitive, m_policy->TheDtoDeserializer());
        CommandBus::Post(std::make_shared<Animators::BuildModelAnimator>(m_animatorPolicy));
    }
    else
    {
        CompleteModelPrimitive();
    }
}

void ModelPrimitiveBuilder::CompleteModelPrimitive()
{
    if (!m_policy) return;
    EventPublisher::Post(std::make_shared<ModelPrimitiveBuilt>(m_buildingRuid, m_policy->Name(), m_builtPrimitive));
}

void ModelPrimitiveBuilder::OnMeshPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::MeshPrimitiveBuilt, IEvent>(e);
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
                if (auto node = m_builtPrimitive->GetMeshNodeTree().GetMeshNode(index.value()))
                {
                    node.value().get().SetMeshPrimitive(mesh_prim);
                    has_mesh_built = true;
                }
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
    const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::BuildMeshPrimitiveFailed, IEvent>(e);
    if (!ev) return;
    for (auto& meta : m_meshBuildingMetas)
    {
        if (meta.m_ruidPolicy == ev->GetRuid())
        {
            EventPublisher::Post(std::make_shared<BuildModelPrimitiveFailed>(m_buildingRuid, m_policy->Name(), ev->GetErrorCode()));
            return;
        }
    }
}

void ModelPrimitiveBuilder::OnModelAnimatorBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!m_animatorPolicy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Animators::ModelAnimatorBuilt>(e);
    if (!ev) return;
    if (m_animatorPolicy->GetRuid() != ev->GetRuid()) return;
    //auto anim = ev->GetAnimator();
    //anim->SetControlledModel(m_builtPrimitive);
    CompleteModelPrimitive();
}

void ModelPrimitiveBuilder::OnBuildModelAnimatorFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!m_animatorPolicy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Animators::BuildModelAnimatorFailed>(e);
    if (!ev) return;
    if (m_animatorPolicy->GetRuid() != ev->GetRuid()) return;
    EventPublisher::Post(std::make_shared<BuildModelPrimitiveFailed>(m_buildingRuid, m_policy->Name(), ev->GetErrorCode()));
}

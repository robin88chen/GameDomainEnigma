#include "ModelPrimitiveBuilder.h"
#include "MeshPrimitiveBuilder.h"
#include "Platforms/MemoryMacro.h"
#include "RenderablePrimitiveDtos.h"
#include "Frameworks/EventPublisher.h"
#include "SkinMeshPrimitive.h"
#include "Frameworks/CommandBus.h"
#include "ModelPrimitiveAnimator.h"
#include "RenderableEvents.h"
#include "GameEngine/FactoryCommands.h"

using namespace Enigma::Renderables;
using namespace Enigma::Frameworks;

ModelPrimitiveBuilder::ModelPrimitiveBuilder() : m_buildingRuid()
{
    m_meshBuilder = menew MeshPrimitiveBuilder();
    m_onMeshPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnMeshPrimitiveBuilt(e); });
    m_onBuildMeshPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildMeshPrimitiveFailed(e); });
    m_onModelAnimatorBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnModelAnimatorBuilt(e); });
    m_onBuildModelAnimatorFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildModelAnimatorFailed(e); });
    EventPublisher::subscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveHydrated), m_onMeshPrimitiveBuilt);
    EventPublisher::subscribe(typeid(MeshPrimitiveBuilder::HydrateMeshPrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    EventPublisher::subscribe(typeid(ModelAnimatorBuilt), m_onModelAnimatorBuilt);
    EventPublisher::subscribe(typeid(BuildModelAnimatorFailed), m_onBuildModelAnimatorFailed);

    //CommandBus::post(std::make_shared<Engine::RegisterDtoPolicyConverter>(ModelPrimitive::TYPE_RTTI.getName(), ModelPrimitiveDto::modelDtoConvertToPolicy));
}

ModelPrimitiveBuilder::~ModelPrimitiveBuilder()
{
    //CommandBus::post(std::make_shared<Engine::UnRegisterDtoPolicyConverter>(ModelPrimitive::TYPE_RTTI.getName()));

    EventPublisher::unsubscribe(typeid(MeshPrimitiveBuilder::MeshPrimitiveHydrated), m_onMeshPrimitiveBuilt);
    EventPublisher::unsubscribe(typeid(MeshPrimitiveBuilder::HydrateMeshPrimitiveFailed), m_onBuildMeshPrimitiveFailed);
    EventPublisher::unsubscribe(typeid(ModelAnimatorBuilt), m_onModelAnimatorBuilt);
    EventPublisher::unsubscribe(typeid(BuildModelAnimatorFailed), m_onBuildModelAnimatorFailed);
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
    m_builtPrimitive = std::make_shared<ModelPrimitive>(m_policy->id());
    for (auto& dto : m_policy->nodeTreeDto().meshNodes())
    {
        MeshNodeDto node_dto = MeshNodeDto::fromGenericDto(dto);
        MeshNode node(node_dto.name());
        node.setT_PosTransform(node_dto.localT_PosTransform());
        node.setLocalTransform(node_dto.localT_PosTransform());
        //node.SetRootRefTransform(node_dto.RootRefTransform());
        if (auto v = node_dto.parentIndexInArray())
        {
            node.setParentIndexInArray(v.value());
        }
        /*if (auto prim = node_dto.meshPrimitive())
        {
            PushInnerMesh(node_dto.name(), std::dynamic_pointer_cast<MeshPrimitivePolicy, Engine::GenericPolicy>(prim->convertToPolicy(m_policy->TheDtoDeserializer())));
        }*/
        m_builtPrimitive->getMeshNodeTree().addMeshNode(node);
    }
    ContinueBuildInnerMesh();
}

void ModelPrimitiveBuilder::PushInnerMesh(const std::string& node_name, const std::shared_ptr<MeshPrimitivePolicy>& policy)
{
    Ruid mesh_ruid = Ruid::generate();
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
    //m_meshBuilder->BuildMeshPrimitive(std::get<Ruid>(m_meshPolicies.front()),
      //  std::get<std::shared_ptr<MeshPrimitivePolicy>>(m_meshPolicies.front()));
    m_meshPolicies.pop();
}

void ModelPrimitiveBuilder::TryBuildAnimator()
{
    if (!m_policy) return;
    if (auto ani = m_policy->modelAnimator())
    {
        //m_animatorPolicy = ani->convertToPolicy(m_builtPrimitive, m_policy->TheDtoDeserializer());
        //CommandBus::post(std::make_shared<Animators::BuildModelAnimator>(m_animatorPolicy));
    }
    else
    {
        CompleteModelPrimitive();
    }
}

void ModelPrimitiveBuilder::CompleteModelPrimitive()
{
    if (!m_policy) return;
    EventPublisher::post(std::make_shared<ModelPrimitiveBuilt>(m_buildingRuid, m_policy->name(), m_builtPrimitive));
}

void ModelPrimitiveBuilder::OnMeshPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::MeshPrimitiveHydrated, IEvent>(e);
    if (!ev) return;
    auto mesh_prim = std::dynamic_pointer_cast<MeshPrimitive>(ev->primitive());
    if (!mesh_prim) return;
    bool has_mesh_built = false;
    for (auto& meta : m_meshBuildingMetas)
    {
        /*if (meta.m_ruidPolicy == ev->ruid())
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
        }*/
    }
    if (has_mesh_built) ContinueBuildInnerMesh();
}

void ModelPrimitiveBuilder::OnBuildMeshPrimitiveFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<MeshPrimitiveBuilder::HydrateMeshPrimitiveFailed, IEvent>(e);
    if (!ev) return;
    for (auto& meta : m_meshBuildingMetas)
    {
        /*if (meta.m_ruidPolicy == ev->ruid())
        {
            EventPublisher::post(std::make_shared<BuildModelPrimitiveFailed>(m_buildingRuid, m_policy->name(), ev->error()));
            return;
        }*/
    }
}

void ModelPrimitiveBuilder::OnModelAnimatorBuilt(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!m_animatorPolicy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<ModelAnimatorBuilt>(e);
    if (!ev) return;
    if (m_animatorPolicy->getRuid() != ev->getRuid()) return;
    //auto anim = ev->getAnimator();
    //anim->SetControlledModel(m_builtPrimitive);
    CompleteModelPrimitive();
}

void ModelPrimitiveBuilder::OnBuildModelAnimatorFailed(const Frameworks::IEventPtr& e)
{
    if (!m_policy) return;
    if (!m_animatorPolicy) return;
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildModelAnimatorFailed>(e);
    if (!ev) return;
    if (m_animatorPolicy->getRuid() != ev->getRuid()) return;
    EventPublisher::post(std::make_shared<BuildModelPrimitiveFailed>(m_buildingRuid, m_policy->name(), ev->GetErrorCode()));
}

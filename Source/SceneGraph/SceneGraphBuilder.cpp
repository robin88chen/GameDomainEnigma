#include "SceneGraphBuilder.h"
#include "SpatialLinkageResolver.h"
#include "SceneGraphRepository.h"
#include "Node.h"
#include "Light.h"
#include "Pawn.h"
#include "LazyNode.h"
#include "VisibilityManagedNode.h"
#include "SceneGraphDtos.h"
#include "SceneGraphEvents.h"
#include "SceneGraphCommands.h"
#include "SceneGraphErrors.h"
#include "PortalZoneNode.h"
#include "Portal.h"
#include "PortalManagementNode.h"
#include "PortalDtos.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/FactoryCommands.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/ResponseBus.h"
#include "Frameworks/RequestBus.h"
#include "Renderer/RenderablePrimitiveRequests.h"
#include "Renderer/RenderablePrimitiveResponses.h"
#include "Renderer/SkinMeshPrimitive.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::Platforms;

SceneGraphBuilder::SceneGraphBuilder(SceneGraphRepository* host, const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer)
{
    m_isCurrentBuilding = false;
    m_host = host;
    m_dtoDeserializer = dto_deserializer;
    m_resolver = menew SpatialLinkageResolver([=](auto n) -> std::shared_ptr<Spatial>
        { return m_host->QuerySpatial(n); });

    m_doRegisteringSpatialFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoRegisteringSpatialFactory(c); });
    CommandBus::Subscribe(typeid(RegisterSpatialDtoFactory), m_doRegisteringSpatialFactory);
    m_doUnRegisteringSpatialFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoUnRegisteringSpatialFactory(c); });
    CommandBus::Subscribe(typeid(UnRegisterSpatialDtoFactory), m_doUnRegisteringSpatialFactory);
    m_doInvokingSpatialDtoFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoInvokingSpatialFactory(c); });
    CommandBus::Subscribe(typeid(InvokeSpatialDtoFactory), m_doInvokingSpatialDtoFactory);

    m_onFactoryCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnFactoryCreated(e); });
    EventPublisher::Subscribe(typeid(FactorySpatialCreated), m_onFactoryCreated);

    CommandBus::Post(std::make_shared<RegisterSpatialDtoFactory>(Light::TYPE_RTTI.GetName(),
        [=](auto dto) { return new Light(dto); }));
    CommandBus::Post(std::make_shared<RegisterSpatialDtoFactory>(Node::TYPE_RTTI.GetName(),
        [=](auto dto) { return new Node(dto); }));
    CommandBus::Post(std::make_shared<RegisterSpatialDtoFactory>(Pawn::TYPE_RTTI.GetName(),
        [=](auto dto) { return new Pawn(dto); }));
    CommandBus::Post(std::make_shared<RegisterSpatialDtoFactory>(LazyNode::TYPE_RTTI.GetName(),
        [=](auto dto) { return new LazyNode(dto); }));
    CommandBus::Post(std::make_shared<RegisterSpatialDtoFactory>(VisibilityManagedNode::TYPE_RTTI.GetName(),
        [=](auto dto) { return new VisibilityManagedNode(dto); }));
    CommandBus::Post(std::make_shared<RegisterSpatialDtoFactory>(PortalZoneNode::TYPE_RTTI.GetName(),
        [=](auto dto) { return new PortalZoneNode(dto); }));
    CommandBus::Post(std::make_shared<RegisterSpatialDtoFactory>(PortalManagementNode::TYPE_RTTI.GetName(),
        [=](auto dto) { return new PortalManagementNode(dto); }));
    CommandBus::Post(std::make_shared<RegisterSpatialDtoFactory>(Portal::TYPE_RTTI.GetName(),
        [=](auto dto) { return new Portal(dto); }));

    m_onBuildPrimitiveResponse = std::make_shared<ResponseSubscriber>([=](auto r) { this->OnBuildPrimitiveResponse(r); });
    ResponseBus::Subscribe(typeid(BuildRenderablePrimitiveResponse), m_onBuildPrimitiveResponse);

    m_doBuildingSceneGraph =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingSceneGraph(c); });
    m_doInPlaceBuildingSceneGraph =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingSceneGraph(c); });
    CommandBus::Subscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
    CommandBus::Subscribe(typeid(SceneGraph::InPlaceBuildSceneGraph), m_doInPlaceBuildingSceneGraph);
}

SceneGraphBuilder::~SceneGraphBuilder()
{
    m_builtSceneGraphMeta.Reset();

    CommandBus::Unsubscribe(typeid(RegisterSpatialDtoFactory), m_doRegisteringSpatialFactory);
    m_doRegisteringSpatialFactory = nullptr;
    CommandBus::Unsubscribe(typeid(UnRegisterSpatialDtoFactory), m_doUnRegisteringSpatialFactory);
    m_doUnRegisteringSpatialFactory = nullptr;
    CommandBus::Unsubscribe(typeid(InvokeSpatialDtoFactory), m_doInvokingSpatialDtoFactory);
    m_doInvokingSpatialDtoFactory = nullptr;

    CommandBus::Unsubscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
    CommandBus::Unsubscribe(typeid(SceneGraph::InPlaceBuildSceneGraph), m_doInPlaceBuildingSceneGraph);
    m_doBuildingSceneGraph = nullptr;
    m_doInPlaceBuildingSceneGraph = nullptr;

    ResponseBus::Unsubscribe(typeid(BuildRenderablePrimitiveResponse), m_onBuildPrimitiveResponse);
    m_onBuildPrimitiveResponse = nullptr;

    CommandBus::Send(std::make_shared<UnRegisterSpatialDtoFactory>(Node::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterSpatialDtoFactory>(Light::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterSpatialDtoFactory>(Pawn::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterSpatialDtoFactory>(LazyNode::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterSpatialDtoFactory>(VisibilityManagedNode::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterSpatialDtoFactory>(PortalZoneNode::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterSpatialDtoFactory>(Portal::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterSpatialDtoFactory>(PortalManagementNode::TYPE_RTTI.GetName()));

    EventPublisher::Unsubscribe(typeid(FactorySpatialCreated), m_onFactoryCreated);
    m_onFactoryCreated = nullptr;

    delete m_resolver;
}

void SceneGraphBuilder::SpatialFactory(const Engine::GenericDto& dto)
{
    auto factory = m_factories.find(dto.GetRtti().GetRttiName());
    if (factory == m_factories.end())
    {
        Debug::Printf("Can't find dto factory of %s\n", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    if (Rtti::IsExactlyOrDerivedFrom(dto.GetRtti().GetRttiName(), Pawn::TYPE_RTTI.GetName()))
    {
        PawnFactory(dto);
    }
    else if (Rtti::IsExactlyOrDerivedFrom(dto.GetRtti().GetRttiName(), Node::TYPE_RTTI.GetName()))
    {
        NodeFactory(dto);
    }
    else if (Rtti::IsExactlyOrDerivedFrom(dto.GetRtti().GetRttiName(), Portal::TYPE_RTTI.GetName()))
    {
        PortalFactory(dto);
    }
    else if (Rtti::IsExactlyOrDerivedFrom(dto.GetRtti().GetRttiName(), Light::TYPE_RTTI.GetName()))
    {
        LightFactory(dto);
    }
    else
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for spatial factory\n", dto.GetRtti().GetRttiName().c_str());
    }
}

void SceneGraphBuilder::NodeFactory(const GenericDto& dto)
{
    if (!Rtti::IsExactlyOrDerivedFrom(dto.GetRtti().GetRttiName(), Node::TYPE_RTTI.GetName()))
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for node factory\n", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    std::shared_ptr<Node> node;
    if (dto.GetRtti().GetRttiName() == Node::TYPE_RTTI.GetName())
    {
        assert(!m_host->HasNode(dto.GetName()));  // node name must be unique
        node = std::dynamic_pointer_cast<Node, Spatial>(m_host->AddNewSpatial(m_factories[dto.GetRtti().GetRttiName()](dto)));
    }
    else
    {
        if (m_host->HasNode(dto.GetName()))
        {
            node = m_host->QueryNode(dto.GetName());
        }
        else
        {
            node = std::dynamic_pointer_cast<Node, Spatial>(m_host->AddNewSpatial(m_factories[dto.GetRtti().GetRttiName()](dto)));
        }
    }
    node->ResolveFactoryLinkage(dto, *m_resolver);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, node));
}

void SceneGraphBuilder::LightFactory(const Engine::GenericDto& dto)
{
    if (!Rtti::IsExactlyOrDerivedFrom(dto.GetRtti().GetRttiName(), Light::TYPE_RTTI.GetName()))
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for light factory\n", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    assert(!m_host->HasLight(dto.GetName()));
    auto light = std::dynamic_pointer_cast<Light, Spatial>(m_host->AddNewSpatial(m_factories[dto.GetRtti().GetRttiName()](dto)));
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, light));
}

void SceneGraphBuilder::PawnFactory(const Engine::GenericDto& dto)
{
    if (!Rtti::IsExactlyOrDerivedFrom(dto.GetRtti().GetRttiName(), Pawn::TYPE_RTTI.GetName()))
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for pawn factory\n", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    assert(!m_host->HasPawn(dto.GetName()));
    auto pawn = std::dynamic_pointer_cast<Pawn, Spatial>(m_host->AddNewSpatial(m_factories[dto.GetRtti().GetRttiName()](dto)));
    PawnDto pawn_dto = PawnDto::FromGenericDto(dto);
    if (auto prim = pawn_dto.ThePrimitive())
    {
        BuildPawnPrimitive(pawn, ConvertPrimitivePolicy(pawn, prim.value()));
    }
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, pawn));
}

void SceneGraphBuilder::PortalFactory(const Engine::GenericDto& dto)
{
    if (!Rtti::IsExactlyOrDerivedFrom(dto.GetRtti().GetRttiName(), Portal::TYPE_RTTI.GetName()))
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for portal factory\n", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    assert(!m_host->HasPortal(dto.GetName()));
    auto portal = std::dynamic_pointer_cast<Portal, Spatial>(m_host->AddNewSpatial(m_factories[dto.GetRtti().GetRttiName()](dto)));
    portal->ResolveFactoryLinkage(dto, *m_resolver);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, portal));
}

void SceneGraphBuilder::BuildSceneGraph(const std::string& scene_graph_id, const GenericDtoCollection& dtos)
{
    m_isCurrentBuilding = true;
    if (m_resolver) m_resolver->ClearResolvers();
    m_builtSceneGraphMeta = BuiltSceneGraphMeta{ scene_graph_id, nullptr, {} };
    for (auto& o : dtos)
    {
        m_builtSceneGraphMeta.m_builtSpatialMetas.emplace_back(BuiltSpatialMeta{ o, std::nullopt });
        CommandBus::Post(std::make_shared<InvokeSpatialDtoFactory>(o));
    }
}

void SceneGraphBuilder::InPlaceBuildSceneGraph(const std::shared_ptr<Node>& sub_root, const GenericDtoCollection& dtos)
{
    if (!sub_root) return;
    m_isCurrentBuilding = true;
    if (m_resolver) m_resolver->ClearResolvers();
    m_builtSceneGraphMeta = BuiltSceneGraphMeta{ sub_root->GetSpatialName(), sub_root, {} };
    for (auto& o : dtos)
    {
        m_builtSceneGraphMeta.m_builtSpatialMetas.emplace_back(BuiltSpatialMeta{ o, std::nullopt });
        CommandBus::Post(std::make_shared<InvokeSpatialDtoFactory>(o));
    }
}

void SceneGraphBuilder::DoBuildingSceneGraph(const ICommandPtr& c)
{
    if (!c) return;
    std::lock_guard locker{ m_buildCommandsLock };
    m_buildCommands.push_back(c);
    if (!m_isCurrentBuilding) BuildNextSceneGraph();
}

void SceneGraphBuilder::BuildNextSceneGraph()
{
    if (m_buildCommands.empty()) return;
    std::lock_guard locker{ m_buildCommandsLock };
    auto c = m_buildCommands.front();
    m_buildCommands.pop_front();
    if (auto cmd = std::dynamic_pointer_cast<SceneGraph::BuildSceneGraph, ICommand>(c))
    {
        BuildSceneGraph(cmd->GetSceneGraphId(), cmd->GetDtos());
    }
    else if (auto cmd = std::dynamic_pointer_cast<SceneGraph::InPlaceBuildSceneGraph, ICommand>(c))
    {
        InPlaceBuildSceneGraph(cmd->GetOwnerNode(), cmd->GetDtos());
    }
}

void SceneGraphBuilder::OnFactoryCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySpatialCreated, IEvent>(e);
    if (!ev) return;
    for (auto& meta : m_builtSceneGraphMeta.m_builtSpatialMetas)
    {
        if (meta.m_dto == ev->GetDto())
        {
            meta.m_spatial = ev->GetSpatial();
            break;
        }
    }
    TryCompleteSceneGraphBuilding();
}

void SceneGraphBuilder::TryCompleteSceneGraphBuilding()
{
    std::vector<std::shared_ptr<Spatial>> top_levels;
    for (auto meta : m_builtSceneGraphMeta.m_builtSpatialMetas)
    {
        if (!meta.m_spatial) return;
        if (meta.m_dto.IsTopLevel())
        {
            top_levels.emplace_back(meta.m_spatial.value());
        }
    }
    auto unresolved_names = m_resolver->GetUnresolvedNames();
    for (auto name : unresolved_names)
    {
        Debug::ErrorPrintf("unresolved name %s\n", name.c_str());
    }
    m_isCurrentBuilding = false;
    if (m_builtSceneGraphMeta.m_in_placeRoot == nullptr)
    {
        EventPublisher::Post(std::make_shared<FactorySceneGraphBuilt>(m_builtSceneGraphMeta.m_sceneGraphId, top_levels));
    }
    else
    {
        EventPublisher::Post(std::make_shared<InPlaceSceneGraphBuilt>(m_builtSceneGraphMeta.m_in_placeRoot));
    }
    BuildNextSceneGraph();
}

std::shared_ptr<RenderablePrimitivePolicy> SceneGraphBuilder::ConvertPrimitivePolicy(const std::shared_ptr<Pawn>& pawn, const Engine::GenericDto& primitive_dto)
{
    if (auto p = primitive_dto.ConvertToPolicy(m_dtoDeserializer))
    {
        return std::dynamic_pointer_cast<RenderablePrimitivePolicy, GenericPolicy>(p);
    }
    else
    {
        EventPublisher::Post(std::make_shared<BuildPawnPrimitiveFailed>(pawn, ErrorCode::unsupportPawnPrimitive));
    }
    return nullptr;
}

void SceneGraphBuilder::BuildPawnPrimitive(const std::shared_ptr<Pawn>& pawn, const std::shared_ptr<RenderablePrimitivePolicy>& primitive_policy)
{
    assert(pawn);
    if (!primitive_policy) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto request = std::make_shared<RequestBuildRenderablePrimitive>(primitive_policy);
    m_buildingPawnPrimitives.insert({ request->GetRuid(), pawn->GetSpatialName() });
    RequestBus::Post(request);
}

void SceneGraphBuilder::OnBuildPrimitiveResponse(const Frameworks::IResponsePtr& r)
{
    if (!r) return;
    auto resp = std::dynamic_pointer_cast<BuildRenderablePrimitiveResponse, IResponse>(r);
    if (!resp) return;
    if (m_buildingPawnPrimitives.empty()) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto it = m_buildingPawnPrimitives.find(resp->GetRequestRuid());
    if (it == m_buildingPawnPrimitives.end()) return;
    if (resp->GetErrorCode())
    {
        Debug::ErrorPrintf("pawn primitive %s build failed : %s\n",
            resp->GetName().c_str(), resp->GetErrorCode().message().c_str());
        if (auto pawn = m_host->QueryPawn(it->second))
        {
            EventPublisher::Post(std::make_shared<BuildPawnPrimitiveFailed>(pawn, resp->GetErrorCode()));
        }
    }
    else
    {
        if (auto pawn = m_host->QueryPawn(it->second))
        {
            pawn->SetPrimitive(resp->GetPrimitive());
            EventPublisher::Post(std::make_shared<PawnPrimitiveBuilt>(pawn));
        }
    }
    m_buildingPawnPrimitives.erase(it);
}

void SceneGraphBuilder::DoRegisteringSpatialFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterSpatialDtoFactory, ICommand>(c);
    if (!cmd) return;
    m_factories.insert_or_assign(cmd->GetRtti(), cmd->GetFactory());
}

void SceneGraphBuilder::DoUnRegisteringSpatialFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterSpatialDtoFactory, ICommand>(c);
    if (!cmd) return;
    if (m_factories.find(cmd->GetRtti()) != m_factories.end()) m_factories.erase(cmd->GetRtti());
}

void SceneGraphBuilder::DoInvokingSpatialFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<InvokeSpatialDtoFactory, ICommand>(c);
    if (!cmd) return;
    SpatialFactory(cmd->GetDto());
}

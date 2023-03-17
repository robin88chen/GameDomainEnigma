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

SceneGraphBuilder::SceneGraphBuilder(SceneGraphRepository* host, const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer,
    const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer)
{
    m_isCurrentBuilding = false;
    m_host = host;
    m_dtoDeserializer = dto_deserializer;
    m_effectDeserializer = effect_deserializer;
    m_resolver = menew SpatialLinkageResolver([=](auto n) -> std::shared_ptr<Spatial>
        { return m_host->QuerySpatial(n); });

    m_onFactoryCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnFactoryCreated(e); });
    EventPublisher::Subscribe(typeid(FactorySpatialCreated), m_onFactoryCreated);

    CommandBus::Post(std::make_shared<RegisterDtoFactory>(Node::TYPE_RTTI.GetName(),
        [=](auto c) { this->NodeFactory(c); }));
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(Light::TYPE_RTTI.GetName(),
        [=](auto c) { this->LightFactory(c); }));
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(Pawn::TYPE_RTTI.GetName(),
        [=](auto c) { this->PawnFactory(c); }));
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(LazyNode::TYPE_RTTI.GetName(),
        [=](auto c) { this->LazyNodeFactory(c); }));
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(VisibilityManagedNode::TYPE_RTTI.GetName(),
        [=](auto c) { this->VisibilityManagedNodeFactory(c); }));
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(PortalZoneNode::TYPE_RTTI.GetName(),
        [=](auto c) { this->PortalZoneNodeFactory(c); }));
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(Portal::TYPE_RTTI.GetName(),
        [=](auto c) { this->PortalFactory(c); }));
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(PortalManagementNode::TYPE_RTTI.GetName(),
        [=](auto c) { this->PortalManagementNodeFactory(c); }));

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

    CommandBus::Unsubscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
    CommandBus::Unsubscribe(typeid(SceneGraph::InPlaceBuildSceneGraph), m_doInPlaceBuildingSceneGraph);
    m_doBuildingSceneGraph = nullptr;
    m_doInPlaceBuildingSceneGraph = nullptr;

    ResponseBus::Unsubscribe(typeid(BuildRenderablePrimitiveResponse), m_onBuildPrimitiveResponse);
    m_onBuildPrimitiveResponse = nullptr;

    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Node::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Light::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Pawn::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(LazyNode::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(VisibilityManagedNode::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(PortalZoneNode::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Portal::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(PortalManagementNode::TYPE_RTTI.GetName()));

    EventPublisher::Unsubscribe(typeid(FactorySpatialCreated), m_onFactoryCreated);
    m_onFactoryCreated = nullptr;

    delete m_resolver;
}

void SceneGraphBuilder::NodeFactory(const GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != Node::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for node factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    NodeDto node_dto = NodeDto::FromGenericDto(dto);
    assert(!m_host->HasNode(node_dto.Name()));
    auto node = m_host->CreateNode(node_dto);
    node->ResolveFactoryLinkage(node_dto, *m_resolver);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, node));
}

void SceneGraphBuilder::LazyNodeFactory(const GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != LazyNode::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for lazy node factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    LazyNodeDto node_dto = LazyNodeDto::FromGenericDto(dto);
    assert(!m_host->HasNode(node_dto.Name()));
    auto node = m_host->CreateLazyNode(node_dto);
    node->ResolveFactoryLinkage(node_dto, *m_resolver);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, node));
}

void SceneGraphBuilder::VisibilityManagedNodeFactory(const GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != VisibilityManagedNode::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for visibility managed node factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    VisibilityManagedNodeDto node_dto = VisibilityManagedNodeDto::FromGenericDto(dto);
    std::shared_ptr<Node> node;
    if (m_host->HasNode(node_dto.Name()))
    {
        node = m_host->QueryNode(node_dto.Name());
    }
    else
    {
        node = m_host->CreateVisibilityManagedNode(node_dto);
    }
    assert(node);
    node->ResolveFactoryLinkage(node_dto, *m_resolver);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, node));
}

void SceneGraphBuilder::LightFactory(const Engine::GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != Light::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for light factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    LightDto light_dto = LightDto::FromGenericDto(dto);
    assert(!m_host->HasLight(light_dto.Name()));
    auto light = m_host->CreateLight(light_dto);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, light));
}

void SceneGraphBuilder::PawnFactory(const Engine::GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != Pawn::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for pawn factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    PawnDto pawn_dto = PawnDto::FromGenericDto(dto);
    assert(!m_host->HasPawn(pawn_dto.Name()));
    auto pawn = m_host->CreatePawn(pawn_dto);
    if (pawn_dto.ThePrimitive())
    {
        BuildPawnPrimitive(pawn, ConvertPrimitivePolicy(pawn, pawn_dto.ThePrimitive().value()));
    }
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, pawn));
}

void SceneGraphBuilder::PortalZoneNodeFactory(const Engine::GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != PortalZoneNode::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for portal zone node factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    PortalZoneNodeDto node_dto = PortalZoneNodeDto::FromGenericDto(dto);
    std::shared_ptr<Node> node;
    if (m_host->HasNode(node_dto.Name()))
    {
        node = m_host->QueryNode(node_dto.Name());
    }
    else
    {
        node = m_host->CreatePortalZoneNode(node_dto);
    }
    assert(node);
    node->ResolveFactoryLinkage(node_dto, *m_resolver);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, node));
}

void SceneGraphBuilder::PortalFactory(const Engine::GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != Portal::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for portal factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    PortalDto portal_dto = PortalDto::FromGenericDto(dto);
    assert(!m_host->HasPortal(portal_dto.Name()));
    auto portal = m_host->CreatePortal(portal_dto);
    portal->ResolveFactoryLinkage(portal_dto.AdjacentZoneNodeName(), *m_resolver);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, portal));
}

void SceneGraphBuilder::PortalManagementNodeFactory(const Engine::GenericDto& dto)
{
    if (dto.GetRtti().GetRttiName() != PortalManagementNode::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for portal management node factory", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    PortalManagementNodeDto node_dto = PortalManagementNodeDto::FromGenericDto(dto);
    std::shared_ptr<PortalManagementNode> node;
    if (m_host->HasNode(node_dto.Name()))
    {
        node = std::dynamic_pointer_cast<PortalManagementNode, Node>(m_host->QueryNode(node_dto.Name()));
    }
    else
    {
        node = std::dynamic_pointer_cast<PortalManagementNode, Node>(m_host->CreatePortalManagementNode(node_dto));
    }
    assert(node);
    node->ResolveFactoryLinkage(node_dto.OutsideZoneNodeName(), *m_resolver);
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, node));
}

void SceneGraphBuilder::BuildSceneGraph(const std::string& scene_graph_id, const std::vector<Engine::GenericDto>& dtos)
{
    m_isCurrentBuilding = true;
    if (m_resolver) m_resolver->ClearResolvers();
    m_builtSceneGraphMeta = BuiltSceneGraphMeta{ scene_graph_id, nullptr, {} };
    for (auto& o : dtos)
    {
        m_builtSceneGraphMeta.m_builtSpatialMetas.emplace_back(BuiltSpatialMeta{ o, std::nullopt });
        CommandBus::Post(std::make_shared<InvokeDtoFactory>(o));
    }
}

void SceneGraphBuilder::InPlaceBuildSceneGraph(const std::shared_ptr<Node>& sub_root, const std::vector<Engine::GenericDto>& dtos)
{
    if (!sub_root) return;
    m_isCurrentBuilding = true;
    if (m_resolver) m_resolver->ClearResolvers();
    m_builtSceneGraphMeta = BuiltSceneGraphMeta{ sub_root->GetSpatialName(), sub_root, {} };
    for (auto& o : dtos)
    {
        m_builtSceneGraphMeta.m_builtSpatialMetas.emplace_back(BuiltSpatialMeta{ o, std::nullopt });
        CommandBus::Post(std::make_shared<InvokeDtoFactory>(o));
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
    if (primitive_dto.GetRtti().GetRttiName() == ModelPrimitive::TYPE_RTTI.GetName())
    {
        ModelPrimitiveDto model = ModelPrimitiveDto::FromGenericDto(primitive_dto);
        return model.ConvertToPolicy(m_dtoDeserializer, m_effectDeserializer);
    }
    else if (primitive_dto.GetRtti().GetRttiName() == MeshPrimitive::TYPE_RTTI.GetName())
    {
        MeshPrimitiveDto mesh = MeshPrimitiveDto::FromGenericDto(primitive_dto);
        return mesh.ConvertToPolicy(m_dtoDeserializer, m_effectDeserializer);
    }
    else if (primitive_dto.GetRtti().GetRttiName() == SkinMeshPrimitive::TYPE_RTTI.GetName())
    {
        SkinMeshPrimitiveDto mesh = SkinMeshPrimitiveDto::FromGenericDto(primitive_dto);
        return mesh.ConvertToPolicy(m_dtoDeserializer, m_effectDeserializer);
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

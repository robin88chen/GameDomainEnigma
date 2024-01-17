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
#include "Platforms/MemoryMacro.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/ResponseBus.h"
#include "Frameworks/RequestBus.h"
#include "Renderer/RenderableCommands.h"
#include "Renderer/RenderableEvents.h"
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
        { return m_host->querySpatial(n); });

    m_doRegisteringSpatialFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoRegisteringSpatialFactory(c); });
    CommandBus::subscribe(typeid(RegisterSpatialDtoFactory), m_doRegisteringSpatialFactory);
    m_doUnRegisteringSpatialFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoUnRegisteringSpatialFactory(c); });
    CommandBus::subscribe(typeid(UnRegisterSpatialDtoFactory), m_doUnRegisteringSpatialFactory);
    m_doInvokingSpatialDtoFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoInvokingSpatialFactory(c); });
    CommandBus::subscribe(typeid(InvokeSpatialDtoFactory), m_doInvokingSpatialDtoFactory);

    m_onFactoryCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnFactoryCreated(e); });
    EventPublisher::subscribe(typeid(FactorySpatialCreated), m_onFactoryCreated);

    CommandBus::post(std::make_shared<RegisterSpatialDtoFactory>(Light::TYPE_RTTI.getName(),
        [=](auto dto) { return new Light(dto); }));
    CommandBus::post(std::make_shared<RegisterSpatialDtoFactory>(Node::TYPE_RTTI.getName(),
        [=](auto dto) { return new Node(dto); }));
    //CommandBus::post(std::make_shared<RegisterSpatialDtoFactory>(Pawn::TYPE_RTTI.getName(),
      //  [=](auto dto) { return new Pawn(dto); }));
    CommandBus::post(std::make_shared<RegisterSpatialDtoFactory>(LazyNode::TYPE_RTTI.getName(),
        [=](auto dto) { return new LazyNode(dto); }));
    CommandBus::post(std::make_shared<RegisterSpatialDtoFactory>(VisibilityManagedNode::TYPE_RTTI.getName(),
        [=](auto dto) { return new VisibilityManagedNode(dto); }));
    CommandBus::post(std::make_shared<RegisterSpatialDtoFactory>(PortalZoneNode::TYPE_RTTI.getName(),
        [=](auto dto) { return new PortalZoneNode(dto); }));
    CommandBus::post(std::make_shared<RegisterSpatialDtoFactory>(PortalManagementNode::TYPE_RTTI.getName(),
        [=](auto dto) { return new PortalManagementNode(dto); }));
    CommandBus::post(std::make_shared<RegisterSpatialDtoFactory>(Portal::TYPE_RTTI.getName(),
        [=](auto dto) { return new Portal(dto); }));

    //m_onPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPrimitiveBuilt(e); });
    //EventPublisher::subscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    //m_onBuildPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildPrimitiveFailed(e); });
    //EventPublisher::subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);

    m_doBuildingSceneGraph =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingSceneGraph(c); });
    m_doInPlaceBuildingSceneGraph =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingSceneGraph(c); });
    CommandBus::subscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
    CommandBus::subscribe(typeid(SceneGraph::InPlaceBuildSceneGraph), m_doInPlaceBuildingSceneGraph);
}

SceneGraphBuilder::~SceneGraphBuilder()
{
    m_builtSceneGraphMeta.reset();

    CommandBus::unsubscribe(typeid(RegisterSpatialDtoFactory), m_doRegisteringSpatialFactory);
    m_doRegisteringSpatialFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnRegisterSpatialDtoFactory), m_doUnRegisteringSpatialFactory);
    m_doUnRegisteringSpatialFactory = nullptr;
    CommandBus::unsubscribe(typeid(InvokeSpatialDtoFactory), m_doInvokingSpatialDtoFactory);
    m_doInvokingSpatialDtoFactory = nullptr;

    CommandBus::unsubscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
    CommandBus::unsubscribe(typeid(SceneGraph::InPlaceBuildSceneGraph), m_doInPlaceBuildingSceneGraph);
    m_doBuildingSceneGraph = nullptr;
    m_doInPlaceBuildingSceneGraph = nullptr;

    EventPublisher::unsubscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    m_onPrimitiveBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
    m_onBuildPrimitiveFailed = nullptr;

    CommandBus::send(std::make_shared<UnRegisterSpatialDtoFactory>(Node::TYPE_RTTI.getName()));
    CommandBus::send(std::make_shared<UnRegisterSpatialDtoFactory>(Light::TYPE_RTTI.getName()));
    CommandBus::send(std::make_shared<UnRegisterSpatialDtoFactory>(Pawn::TYPE_RTTI.getName()));
    CommandBus::send(std::make_shared<UnRegisterSpatialDtoFactory>(LazyNode::TYPE_RTTI.getName()));
    CommandBus::send(std::make_shared<UnRegisterSpatialDtoFactory>(VisibilityManagedNode::TYPE_RTTI.getName()));
    CommandBus::send(std::make_shared<UnRegisterSpatialDtoFactory>(PortalZoneNode::TYPE_RTTI.getName()));
    CommandBus::send(std::make_shared<UnRegisterSpatialDtoFactory>(Portal::TYPE_RTTI.getName()));
    CommandBus::send(std::make_shared<UnRegisterSpatialDtoFactory>(PortalManagementNode::TYPE_RTTI.getName()));

    EventPublisher::unsubscribe(typeid(FactorySpatialCreated), m_onFactoryCreated);
    m_onFactoryCreated = nullptr;

    delete m_resolver;
}

void SceneGraphBuilder::SpatialFactory(const Engine::GenericDto& dto)
{
    auto factory = m_factories.find(dto.getRtti().GetRttiName());
    if (factory == m_factories.end())
    {
        Debug::Printf("Can't find dto factory of %s\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::post(std::make_shared<CreateFactorySpatialFailed>(dto, ErrorCode::spatialFactoryNotFound));
        InterruptBuildingSceneGraph(ErrorCode::spatialFactoryNotFound);
        return;
    }
    /*if (Rtti::isExactlyOrDerivedFrom(dto.getRtti().GetRttiName(), Pawn::TYPE_RTTI.getName()))
    {
        PawnFactory(dto);
    }
    else*/ if (Rtti::isExactlyOrDerivedFrom(dto.getRtti().GetRttiName(), Node::TYPE_RTTI.getName()))
    {
        NodeFactory(dto);
    }
    else if (Rtti::isExactlyOrDerivedFrom(dto.getRtti().GetRttiName(), Portal::TYPE_RTTI.getName()))
    {
        PortalFactory(dto);
    }
    else if (Rtti::isExactlyOrDerivedFrom(dto.getRtti().GetRttiName(), Light::TYPE_RTTI.getName()))
    {
        LightFactory(dto);
    }
    else
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for spatial factory\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::post(std::make_shared<CreateFactorySpatialFailed>(dto, ErrorCode::factoryRttiMismatch));
        InterruptBuildingSceneGraph(ErrorCode::factoryRttiMismatch);
    }
}

void SceneGraphBuilder::NodeFactory(const GenericDto& dto)
{
    if (!Rtti::isExactlyOrDerivedFrom(dto.getRtti().GetRttiName(), Node::TYPE_RTTI.getName()))
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for node factory\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::post(std::make_shared<CreateFactorySpatialFailed>(dto, ErrorCode::factoryRttiMismatch));
        InterruptBuildingSceneGraph(ErrorCode::factoryRttiMismatch);
        return;
    }
    std::shared_ptr<Node> node;
    if (dto.getRtti().GetRttiName() == Node::TYPE_RTTI.getName())
    {
        assert(!m_host->hasNode(dto.getName()));  // node name must be unique
        node = std::dynamic_pointer_cast<Node, Spatial>(m_host->AddNewSpatial(m_factories[dto.getRtti().GetRttiName()](dto)));
    }
    else
    {
        if (m_host->hasNode(dto.getName()))
        {
            node = m_host->queryNode(dto.getName());
        }
        else
        {
            node = std::dynamic_pointer_cast<Node, Spatial>(m_host->AddNewSpatial(m_factories[dto.getRtti().GetRttiName()](dto)));
        }
    }
    node->resolveFactoryLinkage(dto, *m_resolver);
    EventPublisher::post(std::make_shared<FactorySpatialCreated>(dto, node));
}

void SceneGraphBuilder::LightFactory(const Engine::GenericDto& dto)
{
    if (!Rtti::isExactlyOrDerivedFrom(dto.getRtti().GetRttiName(), Light::TYPE_RTTI.getName()))
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for light factory\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::post(std::make_shared<CreateFactorySpatialFailed>(dto, ErrorCode::factoryRttiMismatch));
        InterruptBuildingSceneGraph(ErrorCode::factoryRttiMismatch);
        return;
    }
    assert(!m_host->HasLight(dto.getName()));
    auto light = std::dynamic_pointer_cast<Light, Spatial>(m_host->AddNewSpatial(m_factories[dto.getRtti().GetRttiName()](dto)));
    light->resolveFactoryLinkage(dto, *m_resolver);
    EventPublisher::post(std::make_shared<FactorySpatialCreated>(dto, light));
}

/*void SceneGraphBuilder::PawnFactory(const Engine::GenericDto& dto)
{
    if (!Rtti::isExactlyOrDerivedFrom(dto.getRtti().GetRttiName(), Pawn::TYPE_RTTI.getName()))
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for pawn factory\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::post(std::make_shared<CreateFactorySpatialFailed>(dto, ErrorCode::factoryRttiMismatch));
        InterruptBuildingSceneGraph(ErrorCode::factoryRttiMismatch);
        return;
    }
    assert(!m_host->HasPawn(dto.getName()));
    auto pawn = std::dynamic_pointer_cast<Pawn, Spatial>(m_host->AddNewSpatial(m_factories[dto.getRtti().GetRttiName()](dto)));
    PawnDto pawn_dto = PawnDto::fromGenericDto(dto);
    if (auto prim = pawn_dto.primitive())
    {
        BuildPawnPrimitive(pawn, ConvertPrimitivePolicy(pawn, prim.value()));
    }
    pawn->resolveFactoryLinkage(dto, *m_resolver);
    EventPublisher::post(std::make_shared<FactorySpatialCreated>(dto, pawn));
}*/

void SceneGraphBuilder::PortalFactory(const Engine::GenericDto& dto)
{
    if (!Rtti::isExactlyOrDerivedFrom(dto.getRtti().GetRttiName(), Portal::TYPE_RTTI.getName()))
    {
        Platforms::Debug::ErrorPrintf("wrong dto rtti %s for portal factory\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::post(std::make_shared<CreateFactorySpatialFailed>(dto, ErrorCode::factoryRttiMismatch));
        InterruptBuildingSceneGraph(ErrorCode::factoryRttiMismatch);
        return;
    }
    assert(!m_host->hasPortal(dto.getName()));
    auto portal = std::dynamic_pointer_cast<Portal, Spatial>(m_host->AddNewSpatial(m_factories[dto.getRtti().GetRttiName()](dto)));
    portal->resolveFactoryLinkage(dto, *m_resolver);
    EventPublisher::post(std::make_shared<FactorySpatialCreated>(dto, portal));
}

void SceneGraphBuilder::BuildSceneGraph(const std::string& scene_graph_id, const GenericDtoCollection& dtos)
{
    m_isCurrentBuilding = true;
    if (m_resolver) m_resolver->ClearResolvers();
    m_builtSceneGraphMeta = BuiltSceneGraphMeta{ scene_graph_id, nullptr, {} };
    for (auto& o : dtos)
    {
        m_builtSceneGraphMeta.m_builtSpatialMetas.emplace_back(BuiltSpatialMeta{ o, std::nullopt });
        CommandBus::post(std::make_shared<InvokeSpatialDtoFactory>(o));
    }
}

void SceneGraphBuilder::InPlaceBuildSceneGraph(const std::shared_ptr<Node>& sub_root, const GenericDtoCollection& dtos)
{
    if (!sub_root) return;
    m_isCurrentBuilding = true;
    if (m_resolver) m_resolver->ClearResolvers();
    m_builtSceneGraphMeta = BuiltSceneGraphMeta{ sub_root->getSpatialName(), sub_root, {} };
    for (auto& o : dtos)
    {
        m_builtSceneGraphMeta.m_builtSpatialMetas.emplace_back(BuiltSpatialMeta{ o, std::nullopt });
        CommandBus::post(std::make_shared<InvokeSpatialDtoFactory>(o));
    }
}

void SceneGraphBuilder::InterruptBuildingSceneGraph(error er)
{
    m_isCurrentBuilding = false;
    if (m_builtSceneGraphMeta.m_in_placeRoot == nullptr)
    {
        EventPublisher::post(std::make_shared<BuildFactorySceneGraphFailed>(m_builtSceneGraphMeta.m_sceneGraphId, er));
    }
    else
    {
        EventPublisher::post(std::make_shared<BuildInPlaceSceneGraphFailed>(m_builtSceneGraphMeta.m_in_placeRoot->getSpatialName(), er));
    }
    m_builtSceneGraphMeta.reset();
    BuildNextSceneGraph();
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
        if (meta.m_dto.isTopLevel())
        {
            top_levels.emplace_back(meta.m_spatial.value());
        }
    }
    auto unresolved_names = m_resolver->getUnresolvedNames();
    for (auto name : unresolved_names)
    {
        Debug::ErrorPrintf("unresolved name %s\n", name.c_str());
    }
    m_isCurrentBuilding = false;
    if (m_builtSceneGraphMeta.m_in_placeRoot == nullptr)
    {
        EventPublisher::post(std::make_shared<FactorySceneGraphBuilt>(m_builtSceneGraphMeta.m_sceneGraphId, top_levels));
    }
    else
    {
        EventPublisher::post(std::make_shared<InPlaceSceneGraphBuilt>(m_builtSceneGraphMeta.m_in_placeRoot));
    }
    BuildNextSceneGraph();
}

/*std::shared_ptr<RenderablePrimitivePolicy> SceneGraphBuilder::ConvertPrimitivePolicy(const std::shared_ptr<Pawn>& pawn, const Engine::GenericDto& primitive_dto)
{
    if (auto p = primitive_dto.convertToPolicy(m_dtoDeserializer))
    {
        return std::dynamic_pointer_cast<RenderablePrimitivePolicy, GenericPolicy>(p);
    }
    else
    {
        EventPublisher::post(std::make_shared<BuildPawnPrimitiveFailed>(pawn, ErrorCode::unsupportPawnPrimitive));
    }
    return nullptr;
}*/

/*void SceneGraphBuilder::BuildPawnPrimitive(const std::shared_ptr<Pawn>& pawn, const std::shared_ptr<RenderablePrimitivePolicy>& primitive_policy)
{
    assert(pawn);
    if (!primitive_policy) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto cmd = std::make_shared<BuildRenderablePrimitive>(primitive_policy);
    m_buildingPawnPrimitives.insert({ cmd->getRuid(), pawn->getSpatialName() });
    CommandBus::post(cmd);
}*/

/*void SceneGraphBuilder::OnPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt>(e);
    if (!ev) return;
    if (m_buildingPawnPrimitives.empty()) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto it = m_buildingPawnPrimitives.find(ev->getRequestRuid());
    if (it == m_buildingPawnPrimitives.end()) return;
    if (auto pawn = m_host->QueryPawn(it->second))
    {
        pawn->SetPrimitive(ev->getPrimitive());
        EventPublisher::post(std::make_shared<PawnPrimitiveBuilt>(pawn));
    }
    m_buildingPawnPrimitives.erase(it);
}*/

/*void SceneGraphBuilder::OnBuildPrimitiveFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildRenderablePrimitiveFailed>(e);
    if (!ev) return;
    if (m_buildingPawnPrimitives.empty()) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto it = m_buildingPawnPrimitives.find(ev->getRequestRuid());
    if (it == m_buildingPawnPrimitives.end()) return;
    if (ev->errorCode())
    {
        Debug::ErrorPrintf("pawn primitive %s build failed : %s\n",
            ev->getName().c_str(), ev->errorCode().message().c_str());
        if (auto pawn = m_host->QueryPawn(it->second))
        {
            EventPublisher::post(std::make_shared<BuildPawnPrimitiveFailed>(pawn, ev->errorCode()));
        }
    }
    m_buildingPawnPrimitives.erase(it);
}*/

void SceneGraphBuilder::DoRegisteringSpatialFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterSpatialDtoFactory, ICommand>(c);
    if (!cmd) return;
    m_factories.insert_or_assign(cmd->getRtti(), cmd->GetFactory());
}

void SceneGraphBuilder::DoUnRegisteringSpatialFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterSpatialDtoFactory, ICommand>(c);
    if (!cmd) return;
    if (m_factories.find(cmd->getRtti()) != m_factories.end()) m_factories.erase(cmd->getRtti());
}

void SceneGraphBuilder::DoInvokingSpatialFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<InvokeSpatialDtoFactory, ICommand>(c);
    if (!cmd) return;
    SpatialFactory(cmd->GetDto());
}

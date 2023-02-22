#include "SceneGraphBuilder.h"
#include "SpatialLinkageResolver.h"
#include "SceneGraphRepository.h"
#include "Node.h"
#include "Light.h"
#include "Pawn.h"
#include "SceneGraphDtos.h"
#include "SceneGraphEvents.h"
#include "SceneGraphCommands.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/FactoryCommands.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/RenderablePrimitiveCommands.h"
#include "Renderer/RenderablePrimitiveEvents.h"

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

    m_onPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPrimitiveBuilt(e); });
    m_onBuildPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildPrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);

    m_doBuildingSceneGraph =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingSceneGraph(c); });
    CommandBus::Subscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
}

SceneGraphBuilder::~SceneGraphBuilder()
{
    CommandBus::Unsubscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
    m_doBuildingSceneGraph = nullptr;

    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
    m_onPrimitiveBuilt = nullptr;
    m_onBuildPrimitiveFailed = nullptr;

    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Node::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Light::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Pawn::TYPE_RTTI.GetName()));

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
        BuildPawnPrimitive(pawn, ConvertPrimitivePolicy(pawn_dto.ThePrimitive().value()));
    }
    EventPublisher::Post(std::make_shared<FactorySpatialCreated>(dto, pawn));
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

std::shared_ptr<RenderablePrimitivePolicy> SceneGraphBuilder::ConvertPrimitivePolicy(const Engine::GenericDto& primitive_dto)
{
    if (primitive_dto.GetRtti().GetRttiName() == ModelPrimitive::TYPE_RTTI.GetName())
    {
        ModelPrimitiveDto model = ModelPrimitiveDto::FromGenericDto(primitive_dto);
        return model.ConvertToPolicy(m_dtoDeserializer, m_effectDeserializer);
    }
    //todo : 其他的 primitive 需要嗎??
    return nullptr;
}

void SceneGraphBuilder::BuildPawnPrimitive(const std::shared_ptr<Pawn>& pawn, const std::shared_ptr<RenderablePrimitivePolicy>& primitive_policy)
{
    assert(pawn);
    if (!primitive_policy) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    m_buildingPawnPrimitives.insert({ primitive_policy->GetRuid(), pawn->GetSpatialName() });
    CommandBus::Post(std::make_shared<BuildRenderablePrimitive>(primitive_policy));
}

void SceneGraphBuilder::OnPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    if (m_buildingPawnPrimitives.empty()) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto it = m_buildingPawnPrimitives.find(ev->GetRuid());
    if (it == m_buildingPawnPrimitives.end()) return;
    if (auto pawn = m_host->QueryPawn(it->second))
    {
        pawn->SetPrimitive(ev->GetPrimitive());
    }
    m_buildingPawnPrimitives.erase(it);
}

void SceneGraphBuilder::OnBuildPrimitiveFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<BuildRenderablePrimitiveFailed, IEvent>(e);
    if (!ev) return;
    if (m_buildingPawnPrimitives.empty()) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto it = m_buildingPawnPrimitives.find(ev->GetRuid());
    if (it == m_buildingPawnPrimitives.end()) return;
    Debug::ErrorPrintf("pawn primitive %s build failed : %s\n",
        ev->GetName().c_str(), ev->GetErrorCode().message().c_str());
    m_buildingPawnPrimitives.erase(it);
}

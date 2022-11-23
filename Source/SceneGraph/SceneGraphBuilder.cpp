#include "SceneGraphBuilder.h"
#include "SpatialLinkageResolver.h"
#include "SceneGraphRepository.h"
#include "Node.h"
#include "Light.h"
#include "SceneGraphDtos.h"
#include "SceneGraphEvents.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/FactoryCommands.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

SceneGraphBuilder::SceneGraphBuilder(SceneGraphRepository* host)
{
    m_host = host;
    m_resolver = menew SpatialLinkageResolver([=](auto n) -> std::shared_ptr<Spatial>
        { return m_host->QuerySpatial(n); });

    m_onFactoryCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnFactoryCreated(e); });
    EventPublisher::Subscribe(typeid(FactorySpatialCreated), m_onFactoryCreated);

    CommandBus::Post(std::make_shared<RegisterDtoFactory>(Node::TYPE_RTTI.GetName(),
        [=](auto c) { this->NodeFactory(c); }));
    CommandBus::Post(std::make_shared<RegisterDtoFactory>(Light::TYPE_RTTI.GetName(),
        [=](auto c) { this->LightFactory(c); }));
}

SceneGraphBuilder::~SceneGraphBuilder()
{
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Node::TYPE_RTTI.GetName()));
    CommandBus::Send(std::make_shared<UnRegisterDtoFactory>(Light::TYPE_RTTI.GetName()));

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

void SceneGraphBuilder::BuildSceneGraph(const std::string& scene_graph_id, const std::vector<Engine::GenericDto>& dtos)
{
    if (m_resolver) m_resolver->ClearResolvers();
    m_builtSceneGraphMeta = BuiltSceneGraphMeta{ scene_graph_id, {} };
    for (auto& o : dtos)
    {
        m_builtSceneGraphMeta.m_builtSpatialMetas.emplace_back(BuiltSpatialMeta{ o, std::nullopt });
        CommandBus::Post(std::make_shared<InvokeDtoFactory>(o));
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

    EventPublisher::Post(std::make_shared<FactorySceneGraphBuilt>(m_builtSceneGraphMeta.m_sceneGraphId, top_levels));
}

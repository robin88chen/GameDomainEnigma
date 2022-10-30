#include "SceneGraphBuilder.h"
#include "SpatialLinkageResolver.h"
#include "SceneGraphRepository.h"
#include "Node.h"
#include "SceneGraphContracts.h"
#include "SceneGraphEvents.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/ContractCommands.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

SceneGraphBuilder::SceneGraphBuilder(SceneGraphRepository* host)
{
    m_host = host;
    m_resolver = menew SpatialLinkageResolver([=](auto n) -> std::shared_ptr<Spatial>
        { return m_host->QuerySpatial(n); });

    m_onContractedCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnContractedCreated(e); });
    EventPublisher::Subscribe(typeid(ContractedSpatialCreated), m_onContractedCreated);

    CommandBus::Post(std::make_shared<RegisterContractFactory>(Node::TYPE_RTTI.GetName(),
        [=](auto c) { this->NodeContractFactory(c); }));
}

SceneGraphBuilder::~SceneGraphBuilder()
{
    EventPublisher::Unsubscribe(typeid(ContractedSpatialCreated), m_onContractedCreated);
    m_onContractedCreated = nullptr;

    delete m_resolver;
}

void SceneGraphBuilder::NodeContractFactory(const Contract& contract)
{
    if (contract.GetRtti().GetRttiName() != Node::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong contract rtti %s for node factory", contract.GetRtti().GetRttiName().c_str());
        return;
    }
    NodeContract node_contract = NodeContract::FromContract(contract);
    assert(!m_host->HasNode(node_contract.Name()));
    auto node = m_host->CreateNode(node_contract);
    node->ResolveContractedLinkage(node_contract, *m_resolver);
    EventPublisher::Post(std::make_shared<ContractedSpatialCreated>(contract, node));
}

void SceneGraphBuilder::BuildSceneGraph(const std::string& scene_graph_id, const std::vector<Engine::Contract>& contracts)
{
    if (m_resolver) m_resolver->ClearResolvers();
    m_builtSceneGraphMeta = BuiltSceneGraphMeta{ scene_graph_id, {} };
    for (auto& cont : contracts)
    {
        m_builtSceneGraphMeta.m_builtSpatialMetas.emplace_back(BuiltSpatialMeta{ cont, std::nullopt });
        CommandBus::Post(std::make_shared<InvokeContractFactory>(cont));
    }
}

void SceneGraphBuilder::OnContractedCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<ContractedSpatialCreated, IEvent>(e);
    if (!ev) return;
    for (auto& meta : m_builtSceneGraphMeta.m_builtSpatialMetas)
    {
        if (meta.m_contract == ev->GetContract())
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
        if (meta.m_contract.IsTopLevel())
        {
            top_levels.emplace_back(meta.m_spatial.value());
        }
    }

    EventPublisher::Post(std::make_shared<ContractedSceneGraphBuilt>(m_builtSceneGraphMeta.m_sceneGraphId, top_levels));
}

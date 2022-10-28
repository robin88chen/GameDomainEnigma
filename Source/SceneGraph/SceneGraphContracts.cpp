#include "SceneGraphContracts.h"
#include "Node.h"
#include "GameEngine/BoundingVolumeContract.h"
#include "Spatial.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

std::string TOKEN_NAME = "Name";
std::string TOKEN_LOCAL_TRANSFORM = "LocalTransform";
std::string TOKEN_WORLD_TRANSFORM = "LocalTransform";
std::string TOKEN_GRAPH_DEPTH = "GraphDepth";
std::string TOKEN_WORLD_BOUND = "WorldBound";
std::string TOKEN_MODEL_BOUND = "ModelBound";
std::string TOKEN_CULLING_MODE = "CullingMode";
std::string TOKEN_SPATIAL_FLAG = "SpatialFlag";
std::string TOKEN_NOTIFY_FLAG = "NotifyFlag";
std::string TOKEN_CHILD_NAMES = "ChildNames";

SpatialContract SpatialContract::FromContract(const Contract& contract)
{
    SpatialContract spatial_contract;
    spatial_contract.m_name = contract.TryGetValue<std::string>(TOKEN_NAME).value();
    spatial_contract.m_localTransform = contract.TryGetValue<Matrix4>(TOKEN_LOCAL_TRANSFORM).value();
    spatial_contract.m_worldTransform = contract.TryGetValue<Matrix4>(TOKEN_WORLD_TRANSFORM).value();
    spatial_contract.m_graphDepth = contract.TryGetValue<unsigned int>(TOKEN_GRAPH_DEPTH).value();
    spatial_contract.m_modelBound = contract.TryGetValue<Contract>(TOKEN_MODEL_BOUND).value();
    spatial_contract.m_worldBound = contract.TryGetValue<Contract>(TOKEN_WORLD_BOUND).value();
    spatial_contract.m_cullingMode = contract.TryGetValue<unsigned int>(TOKEN_CULLING_MODE).value();
    spatial_contract.m_spatialFlag = contract.TryGetValue<unsigned int>(TOKEN_SPATIAL_FLAG).value();
    spatial_contract.m_notifyFlag = contract.TryGetValue<unsigned int>(TOKEN_NOTIFY_FLAG).value();
    return spatial_contract;
}

Contract SpatialContract::ToContract()
{
    Contract contract;
    contract.AddRtti(FactoryDesc(Spatial::TYPE_RTTI.GetName()));
    contract.AddOrUpdate(TOKEN_NAME, m_name);
    contract.AddOrUpdate(TOKEN_LOCAL_TRANSFORM, m_localTransform);
    contract.AddOrUpdate(TOKEN_WORLD_TRANSFORM, m_worldTransform);
    contract.AddOrUpdate(TOKEN_GRAPH_DEPTH, m_graphDepth);
    contract.AddOrUpdate(TOKEN_MODEL_BOUND, m_modelBound);
    contract.AddOrUpdate(TOKEN_WORLD_BOUND, m_worldBound);
    contract.AddOrUpdate(TOKEN_CULLING_MODE, m_cullingMode);
    contract.AddOrUpdate(TOKEN_SPATIAL_FLAG, m_spatialFlag);
    contract.AddOrUpdate(TOKEN_NOTIFY_FLAG, m_notifyFlag);
    return contract;
}

NodeContract::NodeContract(const SpatialContract& spatial_contract) : SpatialContract(spatial_contract)
{
}

NodeContract NodeContract::FromContract(const Contract& contract)
{
    NodeContract node_contract(SpatialContract::FromContract(contract));
    node_contract.m_childNames = contract.TryGetValue<std::vector<std::string>>(TOKEN_CHILD_NAMES).value();
    return node_contract;
}

Contract NodeContract::ToContract()
{
    Contract contract = SpatialContract::ToContract();
    contract.AddRtti(FactoryDesc(Node::TYPE_RTTI.GetName()));
    contract.AddOrUpdate(TOKEN_CHILD_NAMES, m_childNames);

    return contract;
}

#include "SceneGraphRepository.h"
#include "Camera.h"
#include "Frustum.h"
#include "Node.h"
#include "Pawn.h"
#include "Light.h"
#include "SceneGraphEvents.h"
#include "SceneGraphDtos.h"
#include "SceneGraphCommands.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraphBuilder.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "Renderer/ModelPrimitive.h"
#include "Renderer/RenderablePrimitiveCommands.h"
#include "Renderer/RenderablePrimitivePolicies.h"
#include "Renderer/RenderablePrimitiveEvents.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Platforms;
using namespace Enigma::Renderer;

DEFINE_RTTI(SceneGraph, SceneGraphRepository, ISystemService);

SceneGraphRepository::SceneGraphRepository(Frameworks::ServiceManager* srv_mngr,
    const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer,
    const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer) : ISystemService(srv_mngr)
{
    m_handSystem = GraphicCoordSys::LeftHand;
    m_dtoDeserializer = dto_deserializer;
    m_effectDeserializer = effect_deserializer;

    m_needTick = false;
    m_builder = menew SceneGraphBuilder(this);
    m_doBuildingSceneGraph =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingSceneGraph(c); });
    CommandBus::Subscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);

    m_onPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnPrimitiveBuilt(e); });
    m_onBuildPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildPrimitiveFailed(e); });
    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
}

SceneGraphRepository::~SceneGraphRepository()
{
    CommandBus::Unsubscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
    m_doBuildingSceneGraph = nullptr;

    EventPublisher::Subscribe(typeid(RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    EventPublisher::Subscribe(typeid(BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
    m_onPrimitiveBuilt = nullptr;
    m_onBuildPrimitiveFailed = nullptr;

    delete m_builder;
}

void SceneGraphRepository::SetCoordinateSystem(GraphicCoordSys hand)
{
    m_handSystem = hand;
}

GraphicCoordSys SceneGraphRepository::GetCoordinateSystem()
{
    return m_handSystem;
}

std::shared_ptr<Camera> SceneGraphRepository::CreateCamera(const std::string& name)
{
    assert(!HasCamera(name));
    auto camera = std::make_shared<Camera>(name, m_handSystem);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert_or_assign(name, camera);
    return camera;
}

bool SceneGraphRepository::HasCamera(const std::string& name)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(name);
    return ((it != m_cameras.end()) && (!it->second.expired()));
}

std::shared_ptr<Camera> SceneGraphRepository::QueryCamera(const std::string& name)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(name);
    if (it == m_cameras.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Frustum> SceneGraphRepository::CreateFrustum(const std::string& name, Frustum::ProjectionType proj)
{
    assert(!HasFrustum(name));
    auto frustum = std::make_shared<Frustum>(name, m_handSystem, proj);
    std::lock_guard locker{ m_frustumMapLock };
    m_frustums.insert_or_assign(name, frustum);
    return frustum;
}

bool SceneGraphRepository::HasFrustum(const std::string& name)
{
    std::lock_guard locker{ m_frustumMapLock };
    auto it = m_frustums.find(name);
    return ((it != m_frustums.end()) && (!it->second.expired()));
}

std::shared_ptr<Frustum> SceneGraphRepository::QueryFrustum(const std::string& name)
{
    std::lock_guard locker{ m_frustumMapLock };
    auto it = m_frustums.find(name);
    if (it == m_frustums.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Node> SceneGraphRepository::CreateNode(const std::string& name)
{
    assert(!HasNode(name));
    auto node = std::make_shared<Node>(name);
    std::lock_guard locker{ m_nodeMapLock };
    m_nodes.insert_or_assign(name, node);
    return node;
}

std::shared_ptr<Node> SceneGraphRepository::CreateNode(const NodeDto& dto)
{
    assert(!HasNode(dto.Name()));
    auto node = std::make_shared<Node>(dto);
    std::lock_guard locker{ m_nodeMapLock };
    m_nodes.insert_or_assign(dto.Name(), node);
    return node;
}

bool SceneGraphRepository::HasNode(const std::string& name)
{
    std::lock_guard locker{ m_nodeMapLock };
    auto it = m_nodes.find(name);
    return ((it != m_nodes.end()) && (!it->second.expired()));
}

std::shared_ptr<Node> SceneGraphRepository::QueryNode(const std::string& name)
{
    std::lock_guard locker{ m_nodeMapLock };
    auto it = m_nodes.find(name);
    if (it == m_nodes.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Pawn> SceneGraphRepository::CreatePawn(const std::string& name)
{
    assert(!HasPawn(name));
    auto pawn = std::make_shared<Pawn>(name);
    std::lock_guard locker{ m_pawnMapLock };
    m_pawns.insert_or_assign(name, pawn);
    return pawn;
}

std::shared_ptr<Pawn> SceneGraphRepository::CreatePawn(const PawnDto& dto)
{
    assert(!HasPawn(dto.Name()));
    auto pawn = std::make_shared<Pawn>(dto);
    std::lock_guard locker{ m_pawnMapLock };
    m_pawns.insert_or_assign(dto.Name(), pawn);
    if (dto.ThePrimitive())
    {
        BuildPawnPrimitive(pawn, ConvertPrimitivePolicy(dto.ThePrimitive().value()));
    }
    return pawn;
}

bool SceneGraphRepository::HasPawn(const std::string& name)
{
    std::lock_guard locker{ m_pawnMapLock };
    auto it = m_pawns.find(name);
    return ((it != m_pawns.end()) && (!it->second.expired()));
}

std::shared_ptr<Pawn> SceneGraphRepository::QueryPawn(const std::string& name)
{
    std::lock_guard locker{ m_pawnMapLock };
    auto it = m_pawns.find(name);
    if (it == m_pawns.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Light> SceneGraphRepository::CreateLight(const std::string& name, const LightInfo& info)
{
    assert(!HasLight(name));
    auto light = std::make_shared<Light>(name, info);
    std::lock_guard locker{ m_lightMapLock };
    m_lights.insert_or_assign(name, light);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoCreated>(light));
    return light;
}

std::shared_ptr<Light> SceneGraphRepository::CreateLight(const LightDto& dto)
{
    assert(!HasLight(dto.Name()));
    auto light = std::make_shared<Light>(dto);
    std::lock_guard locker{ m_lightMapLock };
    m_lights.insert_or_assign(dto.Name(), light);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoCreated>(light));
    return light;
}

bool SceneGraphRepository::HasLight(const std::string& name)
{
    std::lock_guard locker{ m_lightMapLock };
    auto it = m_lights.find(name);
    return ((it != m_lights.end()) && (!it->second.expired()));
}

std::shared_ptr<Light> SceneGraphRepository::QueryLight(const std::string& name)
{
    std::lock_guard locker{ m_lightMapLock };
    auto it = m_lights.find(name);
    if (it == m_lights.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Spatial> SceneGraphRepository::QuerySpatial(const std::string& name)
{
    if (auto node = QueryNode(name)) return node;
    if (auto pawn = QueryPawn(name)) return pawn;
    if (auto light = QueryLight(name)) return light;
    return nullptr;
}

void SceneGraphRepository::DoBuildingSceneGraph(const ICommandPtr& c)
{
    if (!m_builder) return;
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<SceneGraph::BuildSceneGraph, ICommand>(c);
    if (!cmd) return;
    m_builder->BuildSceneGraph(cmd->GetSceneGraphId(), cmd->GetDtos());
}

void SceneGraphRepository::OnPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<RenderablePrimitiveBuilt, IEvent>(e);
    if (!ev) return;
    if (m_buildingPawnPrimitives.empty()) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto it = m_buildingPawnPrimitives.find(ev->GetRuid());
    if (it == m_buildingPawnPrimitives.end()) return;
    if (auto pawn = QueryPawn(it->second))
    {
        pawn->SetPrimitive(ev->GetPrimitive());
    }
    m_buildingPawnPrimitives.erase(it);
}

void SceneGraphRepository::OnBuildPrimitiveFailed(const Frameworks::IEventPtr& e)
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

std::shared_ptr<RenderablePrimitivePolicy> SceneGraphRepository::ConvertPrimitivePolicy(const Engine::GenericDto& primitive_dto)
{
    if (primitive_dto.GetRtti().GetRttiName() == ModelPrimitive::TYPE_RTTI.GetName())
    {
        ModelPrimitiveDto model = ModelPrimitiveDto::FromGenericDto(primitive_dto);
        return model.ConvertToPolicy(m_dtoDeserializer, m_effectDeserializer);
    }
    //todo : 其他的 primitive 需要嗎??
    return nullptr;
}

void SceneGraphRepository::BuildPawnPrimitive(const std::shared_ptr<Pawn>& pawn, const std::shared_ptr<RenderablePrimitivePolicy>& primitive_policy)
{
    assert(pawn);
    if (!primitive_policy) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    m_buildingPawnPrimitives.insert({ primitive_policy->GetRuid(), pawn->GetSpatialName() });
    CommandBus::Post(std::make_shared<BuildRenderablePrimitive>(primitive_policy));
}

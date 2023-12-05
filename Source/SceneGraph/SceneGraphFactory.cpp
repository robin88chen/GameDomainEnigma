#include "SceneGraphFactory.h"
#include "Camera.h"
#include "Pawn.h"
#include "CameraFrustumCommands.h"
#include "SceneGraphCommands.h"
#include "CameraFrustumEvents.h"
#include "SceneGraphDtos.h"
#include "SceneGraphEvents.h"
#include "SceneGraphQueries.h"
#include "SceneGraphErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/RenderableCommands.h"
#include "Renderer/RenderableEvents.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

SceneGraphFactory::SceneGraphFactory()
{

}

SceneGraphFactory::~SceneGraphFactory()
{

}

void SceneGraphFactory::registerHandlers()
{
    m_createCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createCamera(c); });
    CommandBus::subscribe(typeid(CreateCamera), m_createCamera);
    m_constituteCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { constituteCamera(c); });
    CommandBus::subscribe(typeid(ConstituteCamera), m_constituteCamera);
    m_createPawn = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createPawn(c); });
    CommandBus::subscribe(typeid(CreatePawn), m_createPawn);
    m_constitutePawn = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { constitutePawn(c); });
    CommandBus::subscribe(typeid(ConstitutePawn), m_constitutePawn);

    m_onPrimitiveBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->onPrimitiveBuilt(e); });
    EventPublisher::subscribe(typeid(Renderer::RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    m_onBuildPrimitiveFailed = std::make_shared<EventSubscriber>([=](auto e) { this->onBuildPrimitiveFailed(e); });
    EventPublisher::subscribe(typeid(Renderer::BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
}

void SceneGraphFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(CreateCamera), m_createCamera);
    m_createCamera = nullptr;
    CommandBus::unsubscribe(typeid(ConstituteCamera), m_constituteCamera);
    m_constituteCamera = nullptr;
    CommandBus::unsubscribe(typeid(CreatePawn), m_createPawn);
    m_createPawn = nullptr;
    CommandBus::unsubscribe(typeid(ConstitutePawn), m_constitutePawn);
    m_constitutePawn = nullptr;

    EventPublisher::unsubscribe(typeid(Renderer::RenderablePrimitiveBuilt), m_onPrimitiveBuilt);
    m_onPrimitiveBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(Renderer::BuildRenderablePrimitiveFailed), m_onBuildPrimitiveFailed);
    m_onBuildPrimitiveFailed = nullptr;
}

std::shared_ptr<Camera> SceneGraphFactory::createCamera(const SpatialId& id)
{
    return std::make_shared<Camera>(id, GraphicCoordSys::LeftHand);
}

std::shared_ptr<Camera> SceneGraphFactory::constituteCamera(const SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<Camera>(id, dto);
}

void SceneGraphFactory::createCamera(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateCamera>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QueryCamera>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<CreateCameraFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (auto camera = createCamera(cmd->id()))
    {
        EventPublisher::post(std::make_shared<CameraCreated>(cmd->id(), camera));
    }
    else
    {
        EventPublisher::post(std::make_shared<CreateCameraFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

void SceneGraphFactory::constituteCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ConstituteCamera>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QueryCamera>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<ConstituteCameraFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (auto camera = constituteCamera(cmd->id(), cmd->dto()))
    {
        EventPublisher::post(std::make_shared<CameraConstituted>(cmd->id(), camera));
    }
    else
    {
        EventPublisher::post(std::make_shared<ConstituteCameraFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

std::shared_ptr<Pawn> SceneGraphFactory::createPawn(const SpatialId& id)
{
    return std::make_shared<Pawn>(id);
}

std::shared_ptr<Pawn> SceneGraphFactory::constitutePawn(const SpatialId& id, const Engine::GenericDtoCollection& dtos)
{
    auto pawn = std::make_shared<Pawn>(id, dtos[0]);
    PawnDto pawn_dto = PawnDto::fromGenericDto(dtos[0]);
    if (pawn_dto.primitive().has_value())
    {
        auto cmd = std::make_shared<Renderer::BuildRenderablePrimitive>(pawn_dto.primitive().value());
        std::lock_guard<std::recursive_mutex> lock(m_buildingPrimitiveLock);
        m_buildingPawnPrimitives.insert({ cmd->getRuid(), pawn });
        CommandBus::post(cmd);
    }
    return pawn;
}

void SceneGraphFactory::createPawn(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreatePawn>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QueryPawn>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<CreatePawnFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (auto pawn = createPawn(cmd->id()))
    {
        EventPublisher::post(std::make_shared<PawnCreated>(cmd->id(), pawn));
    }
    else
    {
        EventPublisher::post(std::make_shared<CreatePawnFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

void SceneGraphFactory::constitutePawn(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<ConstitutePawn>(c);
    if (!cmd) return;
    const auto query = std::make_shared<QueryPawn>(cmd->id());
    QueryDispatcher::dispatch(query);
    if (query->getResult())
    {
        EventPublisher::post(std::make_shared<ConstitutePawnFailed>(cmd->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    if (auto pawn = constitutePawn(cmd->id(), cmd->dtos()))
    {
        EventPublisher::post(std::make_shared<PawnConstituted>(cmd->id(), pawn));
    }
    else
    {
        EventPublisher::post(std::make_shared<ConstitutePawnFailed>(cmd->id(), ErrorCode::sceneFactoryFailed));
    }
}

void SceneGraphFactory::onPrimitiveBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Renderer::RenderablePrimitiveBuilt>(e);
    if (!ev) return;
    if (m_buildingPawnPrimitives.empty()) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto it = m_buildingPawnPrimitives.find(ev->getRequestRuid());
    if (it == m_buildingPawnPrimitives.end()) return;
    if (!it->second) return;
    it->second->SetPrimitive(ev->getPrimitive());
    EventPublisher::post(std::make_shared<PawnPrimitiveBuilt>(it->second));
    m_buildingPawnPrimitives.erase(it);
}

void SceneGraphFactory::onBuildPrimitiveFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<Renderer::BuildRenderablePrimitiveFailed>(e);
    if (!ev) return;
    if (m_buildingPawnPrimitives.empty()) return;
    std::lock_guard locker{ m_buildingPrimitiveLock };
    auto it = m_buildingPawnPrimitives.find(ev->getRequestRuid());
    if (it == m_buildingPawnPrimitives.end()) return;
    if (ev->errorCode())
    {
        Platforms::Debug::ErrorPrintf("pawn primitive %s build failed : %s\n", ev->getName().c_str(), ev->errorCode().message().c_str());
        if (it->second)
        {
            EventPublisher::post(std::make_shared<BuildPawnPrimitiveFailed>(it->second, ev->errorCode()));
        }
    }
    m_buildingPawnPrimitives.erase(it);
}

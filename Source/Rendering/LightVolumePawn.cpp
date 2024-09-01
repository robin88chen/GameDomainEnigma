#include "LightVolumePawn.h"
#include "Renderables/MeshPrimitive.h"
#include "RenderingErrors.h"
#include "LightingMeshQueries.h"
#include "LightingPawnDto.h"
#include "SceneGraph/LightEvents.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/CameraFrustumEvents.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::Rendering;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Renderables;
using namespace Enigma::MathLib;

DEFINE_RTTI(Rendering, LightVolumePawn, LightingPawn);

std::string LightVolumePawn::m_tokenDefaultVisualTech = "Default";
std::string LightVolumePawn::m_tokenInsideVisualTech = "Inside";

LightVolumePawn::LightVolumePawn(const SpatialId& id) : LightingPawn(id), m_isCameraInside(false)
{
}

LightVolumePawn::LightVolumePawn(const SpatialId& id, const Engine::GenericDto& o) : LightingPawn(id, o), m_isCameraInside(false)
{
    if ((!m_primitive) && (!m_hostLight.expired()))
    {
        LightVolumePawnDto dto(o);
        if ((m_hostLight.lock()->info().lightType() == LightInfo::LightType::Point) && (dto.primitiveId().has_value()))
        {
            m_primitive = std::make_shared<RequestPointLightMeshAssembly>(dto.primitiveId().value(), m_hostLight.lock()->getLightRange())->dispatch();
        }
        m_prensentCameraId = dto.presentCameraId();
    }
}

LightVolumePawn::~LightVolumePawn()
{

}

std::shared_ptr<LightVolumePawn> LightVolumePawn::create(const SceneGraph::SpatialId& id)
{
    return std::make_shared<LightVolumePawn>(id);
}

std::shared_ptr<LightVolumePawn> LightVolumePawn::constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<LightVolumePawn>(id, dto);
}

GenericDto LightVolumePawn::serializeDto()
{
    LightVolumePawnDto dto{ LightingPawnDto(SerializePawnDto()) };
    if (getHostLight()) dto.hostLightId(getHostLight()->id());
    if (!m_prensentCameraId.empty()) dto.presentCameraId(m_prensentCameraId);
    return dto.toGenericDto();
}

void LightVolumePawn::registerHandlers()
{
    LightingPawn::registerHandlers();
    m_onCameraFrameChanged = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { onCameraFrameChanged(e); });
    Frameworks::EventPublisher::subscribe(typeid(CameraFrameChanged), m_onCameraFrameChanged);
}

void LightVolumePawn::unregisterHandlers()
{
    LightingPawn::unregisterHandlers();
    Frameworks::EventPublisher::unsubscribe(typeid(CameraFrameChanged), m_onCameraFrameChanged);
    m_onCameraFrameChanged = nullptr;
}

void LightVolumePawn::toggleCameraInside(bool is_inside)
{
    if (is_inside != m_isCameraInside)
    {
        std::shared_ptr<MeshPrimitive> mesh = std::dynamic_pointer_cast<MeshPrimitive>(getPrimitive());
        if (mesh)
        {
            if (is_inside)
            {
                mesh->selectVisualTechnique(m_tokenInsideVisualTech);
            }
            else
            {
                mesh->selectVisualTechnique(m_tokenDefaultVisualTech);
            }
        }
    }
    m_isCameraInside = is_inside;
}

error LightVolumePawn::_updateSpatialRenderState()
{
    if (!isRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (m_hostLight.expired()) return ErrorCode::nullHostLight;

    Vector4 vecLightPosRange(m_hostLight.lock()->info().position(), m_hostLight.lock()->info().range());
    ColorRGBA colorLight(m_hostLight.lock()->info().color());
    Vector4 vecLightAtten(m_hostLight.lock()->info().attenuation(), 1.0f);
    RenderLightingState lighting_state;
    lighting_state.setPointLightArray({ vecLightPosRange }, { colorLight }, { vecLightAtten });
    m_spatialRenderState = SpatialRenderState(lighting_state);

    return ErrorCode::ok;
}

error LightVolumePawn::_updateLocalTransform(const MathLib::Matrix4& mxLocal)
{
    error er = LightingPawn::_updateLocalTransform(mxLocal);
    checkBackfaceCulling();
    return er;
}

error LightVolumePawn::_updateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    error er = LightingPawn::_updateWorldData(mxParentWorld);
    checkBackfaceCulling();
    return er;
}

void LightVolumePawn::checkBackfaceCulling()
{
    std::shared_ptr<Camera> presentCamera = std::make_shared<QueryRunningCamera>(m_prensentCameraId)->dispatch();
    if (!presentCamera) return;
    const BoundingVolume& bv = getWorldBound();
    if (bv.isEmpty()) return;
    if (bv.PointInside(presentCamera->location()))
    {
        toggleCameraInside(true);
    }
    else
    {
        toggleCameraInside(false);
    }
}

void LightVolumePawn::onLightInfoUpdated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoUpdated>(e);
    if (!ev) return;
    std::shared_ptr<Light> lit = getHostLight();
    if (!lit) return;
    if (ev->light() != getHostLight()) return;
    LightInfoUpdated::NotifyCode notify = ev->notifyCode();
    if (notify == LightInfoUpdated::NotifyCode::Position)
    {
        changeWorldPosition(lit->getLightPosition(), std::nullopt);
    }
    else if (notify == LightInfoUpdated::NotifyCode::Range)
    {
        setLocalUniformScale(lit->getLightRange());
    }
    else if (notify == LightInfoUpdated::NotifyCode::Enable)
    {
        if (lit->info().isEnable())
        {
            removeSpatialFlag(Spatial::Spatial_Hide);
        }
        else
        {
            addSpatialFlag(Spatial::Spatial_Hide);
        }
    }
    else if (notify == LightInfoUpdated::NotifyCode::Color)
    {
        notifySpatialRenderStateChanged();
    }
}

void LightVolumePawn::onCameraFrameChanged(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<CameraFrameChanged>(e);
    if ((!ev) || (!ev->camera())) return;
    if (ev->camera()->id() != m_prensentCameraId) return;
    checkBackfaceCulling();
}

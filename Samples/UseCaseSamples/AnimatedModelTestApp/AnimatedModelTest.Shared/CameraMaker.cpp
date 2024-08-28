#include "CameraMaker.h"
#include "SceneGraph/CameraDtos.h"
#include "MathLib/MathGlobal.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

std::shared_ptr<Camera> CameraMaker::makeCamera(const SpatialId& id)
{
    if (auto cam = Camera::queryCamera(id)) return cam;

    FrustumDto frustum_dto;
    frustum_dto.HandSystem() = GraphicCoordSys::LeftHand;
    frustum_dto.ProjectionType() = Frustum::ProjectionType::Perspective;
    frustum_dto.Fov() = Math::PI / 4.0f;
    frustum_dto.NearPlaneZ() = 0.1f;
    frustum_dto.FarPlaneZ() = 100.0f;
    frustum_dto.AspectRatio() = 4.0f / 3.0f;
    frustum_dto.NearWidth() = 40.0f;
    frustum_dto.NearHeight() = 30.0f;
    CameraDto camera_dto;
    camera_dto.id() = id;
    camera_dto.HandSystem() = GraphicCoordSys::LeftHand;
    camera_dto.EyePosition() = Vector3(-5.0f, 5.0f, -5.0f);
    camera_dto.LookAtDirection() = Vector3(1.0f, -1.0f, 1.0f);
    camera_dto.UpVector() = Vector3::UNIT_Y;
    camera_dto.Frustum() = frustum_dto.toGenericDto();
    camera_dto.factoryDesc() = Enigma::Engine::FactoryDesc(Camera::TYPE_RTTI.getName()).ClaimAsNative(id.name() + ".cam@DataPath");
    return std::make_shared<RequestCameraConstitution>(id, camera_dto.toGenericDto())->dispatch();
}

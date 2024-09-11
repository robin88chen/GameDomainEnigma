#include "CameraMaker.h"
#include "SceneGraph/CameraAssembler.h"
#include "MathLib/MathGlobal.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/CameraFrustumCommands.h"
#include "SceneGraph/FrustumAssembler.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

std::shared_ptr<Camera> CameraMaker::makeCamera(const SpatialId& id)
{
    if (auto cam = Camera::queryCamera(id)) return cam;

    std::shared_ptr<FrustumAssembler> frustum_assembler = std::make_shared<FrustumAssembler>(Frustum::ProjectionType::Perspective);
    frustum_assembler->handSystem(GraphicCoordSys::LeftHand);
    frustum_assembler->fov(Radian(Math::PI / 4.0f));
    frustum_assembler->frontBackZ(0.1f, 100.0f);
    frustum_assembler->nearPlaneDimension(40.0f, 30.0f);
    CameraAssembler camera_assembler(id);
    camera_assembler.handSystem(GraphicCoordSys::LeftHand);
    camera_assembler.eyePosition(Vector3(-5.0f, 5.0f, -5.0f));
    camera_assembler.lookAt(Vector3(1.0f, -1.0f, 1.0f));
    camera_assembler.upDirection(Vector3::UNIT_Y);
    camera_assembler.frustum(frustum_assembler);
    camera_assembler.factoryDesc(Enigma::Engine::FactoryDesc(Camera::TYPE_RTTI.getName()));
    camera_assembler.asNative(id.name() + ".cam@DataPath");
    auto camera = std::make_shared<RequestCameraConstitution>(id, camera_assembler.assemble())->dispatch();
    std::make_shared<PutCamera>(id, camera)->execute();
    return camera;
}

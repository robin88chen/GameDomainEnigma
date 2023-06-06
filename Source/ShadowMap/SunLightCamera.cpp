#include "SunLightCamera.h"

#include "MathLib/MathGlobal.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(ShadowMap, SunLightCamera, Camera);

SunLightCamera::SunLightCamera(const std::string& name) : Camera(name, GraphicCoordSys::LeftHand)
{

}

SunLightCamera::~SunLightCamera()
{

}

void SunLightCamera::SetSunLightDir(const MathLib::Vector3& sun_dir)
{
    m_sunDir = sun_dir;
    m_sunDir.NormalizeSelf();
    MathLib::Vector3 up = MathLib::Vector3::UNIT_Y;
    if ((std::fabs(m_sunDir.Y()) - 1.0f < MathLib::Math::Epsilon())
        && (std::fabs(m_sunDir.Y()) - 1.0f > -MathLib::Math::Epsilon()))
    {
        up = MathLib::Vector3::UNIT_X;
    }
    ChangeCameraFrame(std::nullopt, m_sunDir, up);
}

void SunLightCamera::SetViewerCamera(const std::shared_ptr<Camera>& viewer_camera)
{
    m_viewerCamera = viewer_camera;
}

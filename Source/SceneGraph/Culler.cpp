#include "Culler.h"
#include "Camera.h"
#include "Frustum.h"
#include "SceneGraphErrors.h"
#include "Spatial.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::SceneGraph;

Culler::Culler(const std::shared_ptr<Camera>& camera)
{
    m_isEnableOuterClipping = false;
    m_camera = camera;
    m_countCullerPlane = static_cast<size_t>(CullerPlane::Count);
    m_planeActivations.set();
    m_clipPlanes.resize(m_countCullerPlane);
    m_outerClipPlanes.resize(m_countCullerPlane);
    UpdateFrustumPlanes();
    m_outerClipShiftZ = 2.0f;
}

Culler::Culler(const Culler& culler)
{
    m_isEnableOuterClipping = culler.m_isEnableOuterClipping;
    m_camera = culler.m_camera;
    m_countCullerPlane = culler.m_countCullerPlane;
    m_planeActivations = culler.m_planeActivations;
    m_outerClipShiftZ = culler.m_outerClipShiftZ;
    m_visibleSet = culler.m_visibleSet;
    m_clipPlanes.resize(m_countCullerPlane);
    m_outerClipPlanes.resize(m_countCullerPlane);
    UpdateFrustumPlanes();
}

Culler::Culler(Culler&& culler) noexcept
{
    m_isEnableOuterClipping = culler.m_isEnableOuterClipping;
    m_camera = std::move(culler.m_camera);
    m_countCullerPlane = culler.m_countCullerPlane;
    m_planeActivations = std::move(culler.m_planeActivations);
    m_outerClipShiftZ = culler.m_outerClipShiftZ;
    m_visibleSet = std::move(culler.m_visibleSet);
    m_clipPlanes.resize(m_countCullerPlane);
    m_outerClipPlanes.resize(m_countCullerPlane);
    UpdateFrustumPlanes();
}

Culler::~Culler()
{
}

Culler& Culler::operator=(const Culler& culler)
{
    if (this == &culler) return *this;
    m_isEnableOuterClipping = culler.m_isEnableOuterClipping;
    m_camera = culler.m_camera;
    m_countCullerPlane = culler.m_countCullerPlane;
    m_planeActivations = culler.m_planeActivations;
    m_outerClipShiftZ = culler.m_outerClipShiftZ;
    m_visibleSet = culler.m_visibleSet;
    m_clipPlanes.resize(m_countCullerPlane);
    m_outerClipPlanes.resize(m_countCullerPlane);
    UpdateFrustumPlanes();
    return *this;
}

Culler& Culler::operator=(Culler&& culler) noexcept
{
    m_isEnableOuterClipping = culler.m_isEnableOuterClipping;
    m_camera = std::move(culler.m_camera);
    m_countCullerPlane = culler.m_countCullerPlane;
    m_planeActivations = std::move(culler.m_planeActivations);
    m_outerClipShiftZ = culler.m_outerClipShiftZ;
    m_visibleSet = std::move(culler.m_visibleSet);
    m_clipPlanes.resize(m_countCullerPlane);
    m_outerClipPlanes.resize(m_countCullerPlane);
    UpdateFrustumPlanes();
    return *this;
}

void Culler::SetCamera(const std::shared_ptr<Camera>& camera)
{
    m_camera = camera;
    UpdateFrustumPlanes();
}

void Culler::UpdateFrustumPlanes()
{
    if (!m_camera) return;

    Frustum frustum = m_camera->GetCullingFrustum();

    float xSin, xCos, ySin, yCos;
    if (frustum.GetProjectionType() == Frustum::ProjectionType::Ortho)
    {
        xSin = frustum.GetNearWidth() / 2.0f;
        xCos = frustum.GetNearPlaneZ();
        ySin = frustum.GetNearHeight() / 2.0f;
        yCos = xCos;
        float x = (float)sqrt(xSin * xSin + xCos * xCos);
        float y = (float)sqrt(ySin * ySin + yCos * yCos);
        xSin = xSin / x;
        xCos = xCos / x;
        ySin = ySin / y;
        yCos = yCos / y;
    }
    else
    {
        // m_fFov是 y方向 fov
        float fovy = frustum.GetFov() / 2.0f;
        float fovx = (float)(atan(tan(fovy) * frustum.GetAspectRatio()));
        xCos = (float)cos(fovx);
        xSin = (float)sin(fovx);
        yCos = (float)cos(fovy);
        ySin = (float)sin(fovy);
        if (frustum.GetCoordHandSys() == GraphicCoordSys::RightHand)
        {
            xSin = -xSin;
            ySin = -ySin;
        }
    }

    MathLib::Vector3 vec;

    MathLib::Matrix4 mxCameraWorldTransform = m_camera->GetViewTransform().Inverse();

    MathLib::Vector3 nor, pos;
    // Left Plane
    vec.X() = xCos;
    vec.Y() = 0.0f;
    vec.Z() = xSin;
    std::tie(nor, std::ignore) = mxCameraWorldTransform.TransformVectorNormalized(vec);
    pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, 0.0f));
    m_clipPlanes[static_cast<size_t>(CullerPlane::Left)] = MathLib::Plane3(nor, pos);
    if (m_isEnableOuterClipping)
    {
        pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, -m_outerClipShiftZ));
        m_outerClipPlanes[static_cast<size_t>(CullerPlane::Left)] = MathLib::Plane3(nor, pos);
    }
    // Right Plane
    vec.X() = -xCos;
    vec.Y() = 0.0f;
    vec.Z() = xSin;
    std::tie(nor, std::ignore) = mxCameraWorldTransform.TransformVectorNormalized(vec);
    pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, 0.0f));
    m_clipPlanes[static_cast<size_t>(CullerPlane::Right)] = MathLib::Plane3(nor, pos);
    if (m_isEnableOuterClipping)
    {
        pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, -m_outerClipShiftZ));
        m_outerClipPlanes[static_cast<size_t>(CullerPlane::Right)] = MathLib::Plane3(nor, pos);
    }

    // Top Plane
    vec.X() = 0.0f;
    vec.Y() = -yCos;
    vec.Z() = ySin;
    std::tie(nor, std::ignore) = mxCameraWorldTransform.TransformVectorNormalized(vec);
    pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, 0.0f));
    m_clipPlanes[static_cast<size_t>(CullerPlane::Top)] = MathLib::Plane3(nor, pos);
    if (m_isEnableOuterClipping)
    {
        pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, -m_outerClipShiftZ));
        m_outerClipPlanes[static_cast<size_t>(CullerPlane::Top)] = MathLib::Plane3(nor, pos);
    }

    // Bottom Plane
    vec.X() = 0.0f;
    vec.Y() = yCos;
    vec.Z() = ySin;
    std::tie(nor, std::ignore) = mxCameraWorldTransform.TransformVectorNormalized(vec);
    pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, 0.0f));
    m_clipPlanes[static_cast<size_t>(CullerPlane::Bottom)] = MathLib::Plane3(nor, pos);
    if (m_isEnableOuterClipping)
    {
        pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, -m_outerClipShiftZ));
        m_outerClipPlanes[static_cast<size_t>(CullerPlane::Bottom)] = MathLib::Plane3(nor, pos);
    }

    // Far Plane
    vec.X() = 0.0f;
    vec.Y() = 0.0f;
    vec.Z() = -1.0f;
    float far_z = frustum.GetFarPlaneZ();
    float out_far_z = frustum.GetFarPlaneZ() + m_outerClipShiftZ;
    if (frustum.GetCoordHandSys() == GraphicCoordSys::RightHand)
    {
        vec.Z() = 1.0f;
        far_z = -far_z;
        out_far_z = -out_far_z;
    }
    std::tie(nor, std::ignore) = mxCameraWorldTransform.TransformVectorNormalized(vec);
    pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, far_z));
    m_clipPlanes[static_cast<size_t>(CullerPlane::Back)] = MathLib::Plane3(nor, pos);
    if (m_isEnableOuterClipping)
    {
        pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, out_far_z));
        m_outerClipPlanes[static_cast<size_t>(CullerPlane::Back)] = MathLib::Plane3(nor, pos);
    }

    // Near Plane
    vec.X() = 0.0f;
    vec.Y() = 0.0f;
    vec.Z() = 1.0f;
    float out_near_z = -m_outerClipShiftZ;
    if (frustum.GetCoordHandSys() == GraphicCoordSys::RightHand)
    {
        vec.Z() = -1.0f;
        out_near_z = -out_near_z;
    }
    std::tie(nor, std::ignore) = mxCameraWorldTransform.TransformVectorNormalized(vec);
    pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, 0.0f));
    m_clipPlanes[static_cast<size_t>(CullerPlane::Front)] = MathLib::Plane3(nor, pos);
    if (m_isEnableOuterClipping)
    {
        pos = mxCameraWorldTransform.TransformCoord(MathLib::Vector3(0.0f, 0.0f, out_near_z));
        m_outerClipPlanes[static_cast<size_t>(CullerPlane::Front)] = MathLib::Plane3(nor, pos);
    }
}

error Culler::ComputeVisibleSet(const std::shared_ptr<Spatial>& scene)
{
    m_visibleSet.Clear();

    m_planeActivations.set();

    if (!m_camera) return ErrorCode::cameraNotReady;
    if (!scene) return ErrorCode::nullSceneGraph;
    UpdateFrustumPlanes();

    return scene->CullVisibleSet(this, false);
}

bool Culler::IsVisible(const Engine::BoundingVolume& bound)
{
    if (bound.IsEmpty()) return false;

    // Start with the last pushed plane, which is potentially the most
    // restrictive plane.
    for (int idxPlane = static_cast<int>(m_countCullerPlane - 1); idxPlane >= 0; idxPlane--)
    {
        if (m_planeActivations[idxPlane])
        {
            auto side = bound.SideOfPlane(m_clipPlanes[idxPlane]);

            if (side == Engine::BoundingVolume::Side::Negative)
            {
                // Object is on negative side.  Cull it.
                return false;
            }

            if (side == Engine::BoundingVolume::Side::Positive)
            {
                // Object is on positive side of plane.  There is no need to
                // compare subobjects against this plane, so mark it as
                // inactive.
                m_planeActivations.reset(idxPlane);
            }
        }
    }

    return true;
}

bool Culler::IsVisible(MathLib::Vector3* vecPos, unsigned quantity, bool isIgnoreNearPlane)
{
    // The Boolean variable bIgnoreNearPlane should be set to 'true' when
    // the test polygon is a portal.  This avoids the situation when the
    // portal is in the view pyramid (eye+left/right/top/bottom), but is
    // between the eye and near plane.  In such a situation you do not want
    // the portal system to cull the portal.  This situation typically occurs
    // when the camera moves through the portal from current region to
    // adjacent region.

    // Start with last pushed plane, which is potentially the most
    // restrictive plane.
    unsigned int idxPlane = m_countCullerPlane - 1;
    for (unsigned int i = 0; i < m_countCullerPlane; i++, idxPlane--)
    {
        MathLib::Plane3& plane = m_clipPlanes[idxPlane];
        if (isIgnoreNearPlane && idxPlane == static_cast<unsigned int>(CullerPlane::Front))
        {
            continue;
        }

        unsigned int idxVtx;
        for (idxVtx = 0; idxVtx < quantity; idxVtx++)
        {
            auto side = plane.WhichSide(vecPos[idxVtx]);
            if (side != MathLib::Plane3::SideOfPlane::Negative)
            {
                // polygon is not totally outside this plane
                break;
            }
        }

        if (idxVtx == quantity)
        {
            // polygon is totally outside this plane
            return false;
        }
    }

    return true;
}

bool Culler::IsOutVisibility(const Engine::BoundingVolume& bound)
{
    if (!m_isEnableOuterClipping) return false;  // if not enable, always not
    if (bound.IsEmpty()) return true;  // if no bound, always out

    for (unsigned int i = 0; i < m_countCullerPlane; i++)
    {
        auto side = bound.SideOfPlane(m_outerClipPlanes[i]);

        if (side == Engine::BoundingVolume::Side::Negative)
        {
            // Object is on negative side.  its out.
            return true;
        }
    }

    return false;
}

void Culler::Insert(const std::shared_ptr<Spatial>& obj)
{
    assert(obj);
    m_visibleSet.Insert(obj);
}

void Culler::PushAdditionalPlane(const MathLib::Plane3& plane)
{
    if (m_countCullerPlane < CULLER_MAX_PLANE_QUANTITY)
    {
        m_clipPlanes.push_back(plane);
        m_outerClipPlanes.push_back(plane);
        m_countCullerPlane = static_cast<unsigned>(m_clipPlanes.size());
    }
}

void Culler::RemoveAdditionalPlane()
{
    m_countCullerPlane = static_cast<size_t>(CullerPlane::Count);
    m_clipPlanes.resize(m_countCullerPlane);
    m_outerClipPlanes.resize(m_countCullerPlane);
}

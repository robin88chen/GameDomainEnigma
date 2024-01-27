#include "CSMSunLightCamera.h"
#include "ShadowCasterBoundFilter.h"
#include "MathLib/MathGlobal.h"
#include "MathLib/Triangle3.h"
#include "SceneGraph/Frustum.h"
#include "MathLib/TrianglePlaneClipper.h"
#include "Frameworks/StringFormat.h"

using namespace Enigma::ShadowMap;
using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

DEFINE_RTTI(ShadowMap, CSMSunLightCamera, Camera);

CSMSunLightCamera::CSMSunLightCamera(const SpatialId& id, unsigned partition) : Camera(id, GraphicCoordSys::LeftHand)
{
    m_adjustedViewerNearPlane = 0.1f;
    m_adjustedViewerFarPlane = 10.0f;
    m_partitionCount = 0;
    SetPartitionCount(partition);
}

CSMSunLightCamera::~CSMSunLightCamera()
{
}

void CSMSunLightCamera::SetPartitionCount(unsigned partition)
{
    if (partition == m_partitionCount) return;
    m_partitionCount = partition;
    m_mxLightViewTransforms.resize(partition);
    m_vecLightCameraLocations.resize(partition);
    m_lightFrustums.resize(partition);
    m_mxLightViewProjs.resize(partition);
    m_lightFrustaDistances.resize(partition);
    m_mxSceneCrops.resize(partition);
    m_mxProjSceneCrops.resize(partition);
    m_mxTexCoordTransforms.resize(partition);
}

void CSMSunLightCamera::SetSunLightDir(const Vector3& sun_dir)
{
    m_sunLightDir = sun_dir;
    m_sunLightDir.normalizeSelf();
}

void CSMSunLightCamera::SetViewerCamera(const std::shared_ptr<Camera>& viewer_camera)
{
    m_viewerCamera = viewer_camera;
}

void CSMSunLightCamera::CalcLightCameraSystemMatrix(Culler* culler)
{
    if (!culler) return;

    ShadowCasterBoundFilter filterShadowBound;
    filterShadowBound.computeMergedBound(culler->GetVisibleSet());

    CalcSceneBoundFrustumPlane(culler, filterShadowBound.getMergedBound());

    CalcLightCameraFrustum();

    CalcSceneCropMatrix(filterShadowBound.getMergedBound());

    RefreshTextureCoordTransform();
}

void CSMSunLightCamera::CalcSceneBoundFrustumPlane(SceneGraph::Culler* sceneCuller, const Engine::BoundingVolume& sceneWorldBound)
{
    /*
    這裡在算有效範圍的遠近平面，只有當Frustum完全在scene BB內時，才能直接使用Viewer的遠近平面，
    否則，遠近平面都還有調整的空間，遠近平面的距離是整個SM解析度的關鍵。
    為什麼要切割? 不直接使用scene BB的8個頂點?
    如果 BB 與 frustum Z軸不是垂直的，而且BB比frustum大很多，用BB的角落算出的z值，
    會比實際看得見的範圍的z值差很多
    */
    if (m_viewerCamera.expired()) return;

    m_adjustedViewerNearPlane = m_viewerCamera.lock()->cullingFrustum().nearPlaneZ();
    m_adjustedViewerFarPlane = m_viewerCamera.lock()->cullingFrustum().farPlaneZ();

    if (sceneWorldBound.isEmpty()) return;
    auto cropBox = sceneWorldBound.BoundingBox3();
    if (!cropBox) return;
    // 如果攝影機在BB內，near z 不需調整
    bool isCameraInBox = sceneWorldBound.PointInside(m_viewerCamera.lock()->location());

    auto vecCropBox = cropBox->ComputeVertices();
    unsigned int nFaceIndex[] =
    {
        0,2,1, 0,3,2, 4,5,6, 4,6,7,
        3,6,2, 3,7,6, 0,1,5, 0,5,4,
        4,7,3, 4,3,0, 1,2,6, 1,6,5,
    };
    std::vector<Triangle3> boxTriangles;
    boxTriangles.resize(12);
    for (size_t face = 0; face < 12; face++)
    {
        boxTriangles[face][0] = vecCropBox[nFaceIndex[face * 3]];
        boxTriangles[face][1] = vecCropBox[nFaceIndex[face * 3 + 1]];
        boxTriangles[face][2] = vecCropBox[nFaceIndex[face * 3 + 2]];
    }
    TrianglePlaneClipper tpClipper;
    tpClipper.SetPlanes(sceneCuller->GetPlanes());
    tpClipper.SetTriangles(boxTriangles);
    tpClipper.Clip();
    if (tpClipper.ClippedTriangleCount() == 0) return;

    Matrix4 mxView = m_viewerCamera.lock()->viewTransform();
    Vector3 vecBoxEdgeInView = mxView.TransformCoord(tpClipper.GetResultTriangles()[0][0]);
    Vector3 vecMin = vecBoxEdgeInView;
    Vector3 vecMax = vecBoxEdgeInView;
    for (unsigned int ei = 0; ei < tpClipper.ClippedTriangleCount(); ei++)
    {
        vecBoxEdgeInView = mxView.TransformCoord(tpClipper.GetResultTriangles()[ei][0]);
        vecMin = Math::MinVectorComponent(vecMin, vecBoxEdgeInView);
        vecMax = Math::MaxVectorComponent(vecMax, vecBoxEdgeInView);
        vecBoxEdgeInView = mxView.TransformCoord(tpClipper.GetResultTriangles()[ei][1]);
        vecMin = Math::MinVectorComponent(vecMin, vecBoxEdgeInView);
        vecMax = Math::MaxVectorComponent(vecMax, vecBoxEdgeInView);
        vecBoxEdgeInView = mxView.TransformCoord(tpClipper.GetResultTriangles()[ei][2]);
        vecMin = Math::MinVectorComponent(vecMin, vecBoxEdgeInView);
        vecMax = Math::MaxVectorComponent(vecMax, vecBoxEdgeInView);
    }
    if ((m_adjustedViewerNearPlane < vecMin.z()) && (!isCameraInBox)) m_adjustedViewerNearPlane = vecMin.z();
    if (m_adjustedViewerFarPlane > vecMax.z()) m_adjustedViewerFarPlane = vecMax.z();
}

void CSMSunLightCamera::CalcLightCameraFrustum()
{
    std::array<Vector3, 3> vecLightFrustumAxis = CalcLightCameraFrame();

    for (unsigned frusta = 0; frusta < m_partitionCount; frusta++)
    {
        std::array<Vector3, 8> vecViewerFrustumCorner = CalcViewerFrustumCorner(frusta);
        // light camera's look target is viewer frustum's center
        Vector3 vecViewerFrustumCenter = vecViewerFrustumCorner[0];
        for (unsigned int i = 1; i < 8; i++)
        {
            vecViewerFrustumCenter += vecViewerFrustumCorner[i];
        }
        vecViewerFrustumCenter /= 8.0f;

        Vector3 trans;
        trans.x() = vecLightFrustumAxis[0].dot(vecViewerFrustumCenter);
        trans.y() = vecLightFrustumAxis[1].dot(vecViewerFrustumCenter);
        trans.z() = vecLightFrustumAxis[2].dot(vecViewerFrustumCenter);
        Matrix4 mxLightViewTransform = Matrix4(
            vecLightFrustumAxis[0], vecLightFrustumAxis[1], vecLightFrustumAxis[2], -trans, false);

        // transfer viewer frustum corner to light view, and get max,min vector
        // this light box center is at viewer frustum center
        vecViewerFrustumCorner[0] = mxLightViewTransform.TransformCoord(vecViewerFrustumCorner[0]);
        Vector3 vecMaxLightBox = vecViewerFrustumCorner[0];
        Vector3 vecMinLightBox = vecViewerFrustumCorner[0];
        for (unsigned int i = 1; i < 8; i++)
        {
            vecViewerFrustumCorner[i] = mxLightViewTransform.TransformCoord(vecViewerFrustumCorner[i]);
            vecMaxLightBox = Math::MaxVectorComponent(vecMaxLightBox, vecViewerFrustumCorner[i]);
            vecMinLightBox = Math::MinVectorComponent(vecMinLightBox, vecViewerFrustumCorner[i]);
        }

        // now can get light camera pos
        float lightCameraMoveBack = -vecMinLightBox.z() + 0.2f;  // add some bias
        // move light camera back from dir axis
        Vector3 vecLightCameraPos = vecViewerFrustumCenter - lightCameraMoveBack * m_sunLightDir;
        // and set light camera frame
        SetLightCameraViewTransform(frusta, vecLightCameraPos, vecLightFrustumAxis[2], vecLightFrustumAxis[1], vecLightFrustumAxis[0]);
        // frustum plane w
        float fPlaneW = -vecMinLightBox.x();
        if (fPlaneW < vecMaxLightBox.x()) fPlaneW = vecMaxLightBox.x();
        // frustum plane h
        float fPlaneH = -vecMinLightBox.y();
        if (fPlaneH < vecMaxLightBox.y()) fPlaneH = vecMaxLightBox.y();
        // frustum far z
        float fFarZ = vecMaxLightBox.z() - vecMinLightBox.z() + 0.0f;
        m_lightFrustums[frusta] = Frustum::fromOrtho(m_handSys, fPlaneW * 2.0f, fPlaneH * 2.0f, 0.1f, fFarZ);
        m_mxProjSceneCrops[frusta] = m_mxSceneCrops[frusta] * m_lightFrustums[frusta].projectionTransform();
    }

    RefreshTextureCoordTransform();
}

std::array<Vector3, 3> CSMSunLightCamera::CalcLightCameraFrame() const
{
    std::array<Vector3, 3> vecLightCameraFrame;
    vecLightCameraFrame[2] = m_sunLightDir;
    vecLightCameraFrame[2].normalizeSelf();
    Vector3 vecUp = Vector3::UNIT_Y;
    if ((std::fabs(vecLightCameraFrame[2].y()) - 1.0f < Math::Epsilon())
        && (std::fabs(vecLightCameraFrame[2].y()) - 1.0f > -Math::Epsilon()))
    {
        vecUp = Vector3::UNIT_X;
    }
    vecLightCameraFrame[0] = vecUp.cross(vecLightCameraFrame[2]);
    vecLightCameraFrame[0].normalizeSelf();
    vecLightCameraFrame[1] = vecLightCameraFrame[2].cross(vecLightCameraFrame[0]);
    vecLightCameraFrame[1].normalizeSelf();
    return vecLightCameraFrame;
}

std::array<Vector3, 8> CSMSunLightCamera::CalcViewerFrustumCorner(unsigned frustaIndex)
{
    std::array<Vector3, 8> viewerFrustumCorners;

    if (m_viewerCamera.expired()) return viewerFrustumCorners;
    Frustum frustum = m_viewerCamera.lock()->cullingFrustum();

    float viewerNear = m_adjustedViewerNearPlane;
    float viewerFar = m_adjustedViewerFarPlane;

    float frustaIDM = static_cast<float>(frustaIndex + 1) / static_cast<float>(m_partitionCount);
    float frustaLog = viewerNear * std::powf(viewerFar / viewerNear, frustaIDM);
    float frustaUniform = viewerNear + (viewerFar - viewerNear) * frustaIDM;

    float frustaNear = viewerNear;
    float frustaFar = viewerFar;
    if (frustaIndex != m_partitionCount - 1)  // not last partition
    {
        frustaFar = (frustaLog + frustaUniform) / 2.0f;
    }
    if (frustaIndex != 0)  // not first partition
    {
        frustaNear = m_lightFrustaDistances[frustaIndex - 1];
    }
    if (frustaIndex < m_partitionCount) m_lightFrustaDistances[frustaIndex] = frustaFar;

    Vector3 vecZ = m_viewerCamera.lock()->eyeToLookatVector();
    Vector3 vecX = m_viewerCamera.lock()->rightVector();
    Vector3 vecY = m_viewerCamera.lock()->upVector();
    float frustaAspect = frustum.aspectRatio();
    float frustaFov = frustum.fov();

    float nearPlaneHalfHeight = std::tan(frustaFov * 0.5f) * frustaNear;
    float nearPlaneHalfWidth = nearPlaneHalfHeight * frustaAspect;

    float farPlaneHalfHeight = std::tan(frustaFov * 0.5f) * frustaFar;
    float farPlaneHalfWidth = farPlaneHalfHeight * frustaAspect;

    Vector3 vecNearPlaneCenter = m_viewerCamera.lock()->location() + vecZ * frustaNear;
    Vector3 vecFarPlaneCenter = m_viewerCamera.lock()->location() + vecZ * frustaFar;

    viewerFrustumCorners[0] = vecNearPlaneCenter - vecX * nearPlaneHalfWidth - vecY * nearPlaneHalfHeight;
    viewerFrustumCorners[1] = vecNearPlaneCenter - vecX * nearPlaneHalfWidth + vecY * nearPlaneHalfHeight;
    viewerFrustumCorners[2] = vecNearPlaneCenter + vecX * nearPlaneHalfWidth + vecY * nearPlaneHalfHeight;
    viewerFrustumCorners[3] = vecNearPlaneCenter + vecX * nearPlaneHalfWidth - vecY * nearPlaneHalfHeight;

    viewerFrustumCorners[4] = vecFarPlaneCenter - vecX * farPlaneHalfWidth - vecY * farPlaneHalfHeight;
    viewerFrustumCorners[5] = vecFarPlaneCenter - vecX * farPlaneHalfWidth + vecY * farPlaneHalfHeight;
    viewerFrustumCorners[6] = vecFarPlaneCenter + vecX * farPlaneHalfWidth + vecY * farPlaneHalfHeight;
    viewerFrustumCorners[7] = vecFarPlaneCenter + vecX * farPlaneHalfWidth - vecY * farPlaneHalfHeight;

    return viewerFrustumCorners;
}

void CSMSunLightCamera::SetLightCameraViewTransform(unsigned frustaIndex,
    const MathLib::Vector3& eye, const MathLib::Vector3& dir, const MathLib::Vector3& up, const MathLib::Vector3& right)
{
    assert(frustaIndex < m_partitionCount);
    Vector3 _dir = dir.normalize();
    Vector3 _up = up.normalize();
    Vector3 _right = right.normalize();
    Vector3 trans;
    trans.x() = _right.dot(eye);
    trans.y() = _up.dot(eye);
    trans.z() = _dir.dot(eye);
    m_mxLightViewTransforms[frustaIndex] = Matrix4(_right, _up, _dir, -trans, false);
    m_vecLightCameraLocations[frustaIndex] = eye;
}

void CSMSunLightCamera::RefreshTextureCoordTransform()
{
    assert(m_partitionCount > 0);
    if (m_partitionCount <= 0) return;
    if (m_mxTexCoordTransforms.empty()) return;
    if (m_mxProjSceneCrops.empty()) return;
    if (m_mxLightViewTransforms.empty()) return;

    m_mxTexCoordTransforms[0] = Matrix4::IDENTITY;
    for (unsigned int frusta = 1; frusta < m_partitionCount; frusta++)
    {
        Matrix4 mxLight0 = m_mxProjSceneCrops[0] * m_mxLightViewTransforms[0];
        Matrix4 mxLight0Inv = mxLight0.Inverse();
        Matrix4 mxLightN = m_mxProjSceneCrops[frusta] * m_mxLightViewTransforms[frusta];
        m_mxTexCoordTransforms[frusta] = mxLightN * mxLight0Inv;
    }
}

void CSMSunLightCamera::CalcSceneCropMatrix(const Engine::BoundingVolume& sceneWorldBound)
{
    // https://developer.download.nvidia.com/SDK/10.5/opengl/src/cascaded_shadow_maps/doc/cascaded_shadow_maps.pdf
    // https://github.com/GKR/NvidiaCascadedShadowMapsGLM
    for (unsigned int frusta = 0; frusta < m_partitionCount; frusta++)
    {
        Matrix4 mxLightViewProj = m_lightFrustums[frusta].projectionTransform() * m_mxLightViewTransforms[frusta];

        BoundingVolume cropBound = Engine::BoundingVolume::CreateFromTransform(sceneWorldBound, mxLightViewProj);
        auto cropBox = cropBound.BoundingBox3();
        if (!cropBox) return;
        const auto vecCropBox = cropBox->ComputeVertices();
        assert(vecCropBox.size() == 8);
        Vector3 vecMin = vecCropBox[0];
        Vector3 vecMax = vecCropBox[0];
        for (unsigned int i = 1; i < 8; i++)
        {
            vecMin = Math::MinVectorComponent(vecMin, vecCropBox[i]);
            vecMax = Math::MaxVectorComponent(vecMax, vecCropBox[i]);
        }
        //vecMin = Math::MaxVectorComponent(vecMin, Vector3(-1.0f, -1.0f, 0.0f));
        //vecMax = Math::MinVectorComponent(vecMax, Vector3(1.0f, 1.0f, 1.0f));
        //DebugPrintf("frusta %d, vecMin %f, %f, %f, vecMax %f, %f, %f\n", frusta, vecMin.x(), vecMin.y(), vecMin.z(),
        //    vecMax.x(), vecMax.y(), vecMax.z());
        //vecMin.z() = 0.0f;
        float scaleX, scaleY; // , scaleZ;
        float offsetX, offsetY; // , offsetZ;
        scaleX = 2.0f / (vecMax.x() - vecMin.x());
        scaleY = 2.0f / (vecMax.y() - vecMin.y());
        //scaleZ = 1.0f / (vecMax.z() - vecMin.z());
        offsetX = -0.5f * (vecMax.x() + vecMin.x()) * scaleX;
        offsetY = -0.5f * (vecMax.y() + vecMin.y()) * scaleY;
        //offsetZ = -vecMin.z() * scaleZ;
        m_mxSceneCrops[frusta] = Matrix4(scaleX, 0.0f, 0.0f, offsetX,
            0.0f, scaleY, 0.0f, offsetY,
            0.0f, 0.0f, 1.0f, 0.0f, //scaleZ, offsetZ,
            0.0f, 0.0f, 0.0f, 1.0f);
        m_mxProjSceneCrops[frusta] = m_mxSceneCrops[frusta] * m_lightFrustums[frusta].projectionTransform();
        m_mxLightViewProjs[frusta] = m_mxProjSceneCrops[frusta] * m_mxLightViewTransforms[frusta];
    }
}

const Matrix4& CSMSunLightCamera::GetLightViewTransform(unsigned int index) const
{
    assert(index < m_partitionCount);
    return m_mxLightViewTransforms[index];
}

const Matrix4& CSMSunLightCamera::GetLightProjectionTransform(unsigned int index) const
{
    assert(index < m_partitionCount);
    return m_mxProjSceneCrops[index];
}

const Vector3& CSMSunLightCamera::GetLightCameraLocation(unsigned int index) const
{
    assert(index < m_partitionCount);
    return m_vecLightCameraLocations[index];
}

Vector4 CSMSunLightCamera::LightFrustaDistanceToVector4() const
{
    Vector4 vecLightFrustaDistance = Vector4::ZERO;
    unsigned count = m_partitionCount;
    if (count > 4) count = 4;
    for (unsigned int i = 0; i < count; i++)
    {
        vecLightFrustaDistance[i] = m_lightFrustaDistances[i];
    }
    return vecLightFrustaDistance;
}

/*********************************************************************
 * \file   CSMSunLightCamera.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef CSM_SUN_LIGHT_CAMERA_H
#define CSM_SUN_LIGHT_CAMERA_H

#include "SceneGraph/Camera.h"
#include "SceneGraph/Culler.h"

namespace Enigma::ShadowMap
{
    class CSMSunLightCamera : public SceneGraph::Camera
    {
        DECLARE_EN_RTTI;
    public:
        CSMSunLightCamera(std::string name, unsigned partition = 1);
        virtual ~CSMSunLightCamera() override;
        CSMSunLightCamera(const CSMSunLightCamera& other) = delete;
        CSMSunLightCamera(CSMSunLightCamera&& other) noexcept = delete;
        CSMSunLightCamera& operator=(const CSMSunLightCamera& other) = delete;
        CSMSunLightCamera& operator=(CSMSunLightCamera&& other) noexcept = delete;

        unsigned int GetPartitionCount() const { return m_partitionCount; };
        void SetPartitionCount(unsigned int partition);

        void SetSunLightDir(const MathLib::Vector3& sun_dir);
        void SetViewerCamera(const std::shared_ptr<Camera>& viewer_camera);

        void CalcLightCameraSystemMatrix(SceneGraph::Culler* culler);

        const MathLib::Matrix4& GetLightViewTransform(unsigned int index) const;
        const MathLib::Matrix4& GetLightProjectionTransform(unsigned int index) const;
        const MathLib::Vector3& GetLightCameraLocation(unsigned int index) const;

        const std::vector<MathLib::Matrix4>& GetLightViewProjectionTransforms() const { return m_mxLightViewProjs; };
        const std::vector<float>& GetLightFrustaDistances() const { return m_lightFrustaDistances; };
        const std::vector<MathLib::Matrix4>& GetTextureCoordTransforms() const { return m_mxTexCoordTransforms; };
        MathLib::Vector4 LightFrustaDistanceToVector4() const;

    protected:
        void CalcSceneBoundFrustumPlane(SceneGraph::Culler* sceneCuller, const Engine::BoundingVolume& sceneWorldBound);
        void CalcLightCameraFrustum();
        std::array<MathLib::Vector3, 3> CalcLightCameraFrame() const;
        std::array<MathLib::Vector3, 8> CalcViewerFrustumCorner(unsigned frustaIndex);
        void SetLightCameraViewTransform(unsigned frustaIndex, const MathLib::Vector3& eye, const MathLib::Vector3& dir,
            const MathLib::Vector3& up, const MathLib::Vector3& right);
        void RefreshTextureCoordTransform();
        void CalcSceneCropMatrix(const Engine::BoundingVolume& sceneWorldBound);

    private:
        unsigned m_partitionCount;
        MathLib::Vector3 m_sunLightDir;
        std::weak_ptr<SceneGraph::Camera> m_viewerCamera;

        std::vector<MathLib::Matrix4> m_mxLightViewTransforms;
        std::vector<MathLib::Vector3> m_vecLightCameraLocations;
        std::vector<SceneGraph::FrustumPtr> m_lightFrustums;
        std::vector<MathLib::Matrix4> m_mxLightViewProjs;
        std::vector<float> m_lightFrustaDistances;
        std::vector<MathLib::Matrix4> m_mxSceneCrops;  ///< 對有效的場景做縮放
        std::vector<MathLib::Matrix4> m_mxProjSceneCrops;
        std::vector<MathLib::Matrix4> m_mxTexCoordTransforms;  ///< 每個partition的貼圖座標不用全部算，用第一個partition的貼圖座標，再用矩陣去轉換，節省傳給pixel shader的input資料
        float m_adjustedViewerNearPlane; ///< 調整的遠近平面，讓shadow map 做有效利用
        float m_adjustedViewerFarPlane;
    };
}

#endif // CSM_SUN_LIGHT_CAMERA_H

/*********************************************************************
 * \file   SunLightCamera.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _SUN_LIGHT_CAMERA_H
#define _SUN_LIGHT_CAMERA_H

#include "SceneGraph/Camera.h"

namespace Enigma::ShadowMap
{
    class SunLightCamera : public SceneGraph::Camera
    {
        DECLARE_EN_RTTI;
    public:
        SunLightCamera(const std::string& name);
        virtual ~SunLightCamera() override;
        SunLightCamera(const SunLightCamera& other) = delete;
        SunLightCamera(SunLightCamera&& other) noexcept = delete;
        SunLightCamera& operator=(const SunLightCamera& other) = delete;
        SunLightCamera& operator=(SunLightCamera&& other) noexcept = delete;

        void SetSunLightDir(const MathLib::Vector3& sun_dir);
        void SetViewerCamera(const std::shared_ptr<Camera>& viewer_camera);

    protected:
        MathLib::Vector3 m_sunDir;
        std::weak_ptr<Camera> m_viewerCamera;
    };
}

#endif // _SUN_LIGHT_CAMERA_H

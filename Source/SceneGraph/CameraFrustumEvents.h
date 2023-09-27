/*********************************************************************
 * \file   CameraFrustumEvents.h
 * \brief  camera & frustum events
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _CAMERA_FRUSTUM_EVENTS_H
#define _CAMERA_FRUSTUM_EVENTS_H

#include "Frameworks/Event.h"
#include "Frustum.h"
#include <memory>
#include <system_error>

namespace Enigma::SceneGraph
{
    class Camera;

    class CameraFrameChanged : public Frameworks::IEvent
    {
    public:
        CameraFrameChanged(const std::shared_ptr<Camera>& camera) : m_camera(camera) {};
        std::shared_ptr<Camera> GetCamera() { return m_camera.lock(); }

    protected:
        std::weak_ptr<Camera> m_camera;
    };
    class FrustumShapeChanged : public Frameworks::IEvent
    {
    public:
        FrustumShapeChanged(const Frustum& frustum) : m_frustum(frustum) {};
        const Frustum& GetFrustum() { return m_frustum; }

    protected:
        Frustum m_frustum;
    };

    class CameraCreated : public Frameworks::IEvent
    {
    public:
        CameraCreated(const std::string& name, const std::shared_ptr<Camera>& camera) : m_name(name), m_camera(camera) {};
        const std::string& GetName() const { return m_name; }
        std::shared_ptr<Camera> GetCamera() { return m_camera; }

    protected:
        std::string m_name;
        std::shared_ptr<Camera> m_camera;
    };
    class CreateCameraFailed : public Frameworks::IEvent
    {
    public:
        CreateCameraFailed(const std::string& name, std::error_code err) : m_name(name), m_err(err) {};
        const std::string& GetName() const { return m_name; }
        std::error_code GetError() const { return m_err; }

    protected:
        std::string m_name;
        std::error_code m_err;
    };
    class ReplyCameraQuery : public Frameworks::IResponseEvent
    {
    public:
        ReplyCameraQuery(const Frameworks::Ruid& ruid, const std::shared_ptr<Camera>& camera) : IResponseEvent(ruid), m_camera(camera) {};
        std::shared_ptr<Camera> GetCamera() const { return m_camera.lock(); }

    protected:
        std::weak_ptr<Camera> m_camera;
    };
    class QueryCameraFailed : public Frameworks::IResponseEvent
    {
    public:
        QueryCameraFailed(const Frameworks::Ruid& ruid, std::error_code err) : IResponseEvent(ruid), m_err(err) {};
        std::error_code GetError() const { return m_err; }

    protected:
        std::error_code m_err;
    };
}

#endif // _CAMERA_FRUSTUM_EVENTS_H

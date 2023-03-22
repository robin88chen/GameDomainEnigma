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
#include <memory>

namespace Enigma::SceneGraph
{
    class Camera;
    class Frustum;

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
        FrustumShapeChanged(const std::shared_ptr<Frustum>& frustum) : m_frustum(frustum) {};
        std::shared_ptr<Frustum> GetFrustum() { return m_frustum.lock(); }

    protected:
        std::weak_ptr<Frustum> m_frustum;
    };

}

#endif // _CAMERA_FRUSTUM_EVENTS_H

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
#include "SpatialId.h"
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
        CameraCreated(const SpatialId& id, const std::shared_ptr<Camera>& camera) : m_id(id), m_camera(camera) {};
        const SpatialId& id() const { return m_id; }
        std::shared_ptr<Camera> camera() { return m_camera; }

    protected:
        SpatialId m_id;
        std::shared_ptr<Camera> m_camera;
    };
    class CreateCameraFailed : public Frameworks::IEvent
    {
    public:
        CreateCameraFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    class CameraConstituted : public Frameworks::IEvent
    {
    public:
        CameraConstituted(const SpatialId& id, const std::shared_ptr<Camera>& camera, bool is_persisted) : m_id(id), m_camera(camera), m_is_persisted(is_persisted) {};
        const SpatialId& id() const { return m_id; }
        std::shared_ptr<Camera> camera() { return m_camera; }
        bool isPersisted() const { return m_is_persisted; }

    protected:
        SpatialId m_id;
        std::shared_ptr<Camera> m_camera;
        bool m_is_persisted;
    };
    class ConstituteCameraFailed : public Frameworks::IEvent
    {
    public:
        ConstituteCameraFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    class CameraPut : public Frameworks::IEvent
    {
    public:
        CameraPut(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }
    protected:
        SpatialId m_id;
    };
    class PutCameraFailed : public Frameworks::IEvent
    {
    public:
        PutCameraFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
    class CameraRemoved : public Frameworks::IEvent
    {
    public:
        CameraRemoved(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
    class RemoveCameraFailed : public Frameworks::IEvent
    {
    public:
        RemoveCameraFailed(const SpatialId& id, std::error_code err) : m_id(id), m_err(err) {};
        const SpatialId& id() const { return m_id; }
        std::error_code error() const { return m_err; }

    protected:
        SpatialId m_id;
        std::error_code m_err;
    };
}

#endif // _CAMERA_FRUSTUM_EVENTS_H

/*********************************************************************
 * \file   CameraFrustumCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2023
 *********************************************************************/
#ifndef CAMERA_FRUSTUM_COMMANDS_H
#define CAMERA_FRUSTUM_COMMANDS_H

#include "Frameworks/Command.h"
#include "GameEngine/GenericDto.h"
#include "SpatialId.h"

namespace Enigma::SceneGraph
{
    class PutCamera : public Frameworks::ICommand
    {
    public:
        PutCamera(const SpatialId& id, const std::shared_ptr<Camera>& camera) : m_id(id), m_camera(camera) {}

        const SpatialId& id() { return m_id; }
        const std::shared_ptr<Camera>& camera() { return m_camera; }

    protected:
        SpatialId m_id;
        std::shared_ptr<Camera> m_camera;
    };
    class RemoveCamera : public Frameworks::ICommand
    {
    public:
        RemoveCamera(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() { return m_id; }

    protected:
        SpatialId m_id;
    };
}

#endif // CAMERA_FRUSTUM_COMMANDS_H

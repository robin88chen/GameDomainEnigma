/*********************************************************************
 * \file   SceneGraphDtoHelper.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_DTO_HELPER_H
#define SCENE_GRAPH_DTO_HELPER_H

#include <string>
#include "CameraFrustumDtos.h"

namespace Enigma::SceneGraph
{
    class CameraDtoHelper
    {
    public:
        CameraDtoHelper(const std::string& name);

        CameraDtoHelper& EyePosition(const MathLib::Vector3& eye_position);
        CameraDtoHelper& LookAt(const MathLib::Vector3& look_at);
        CameraDtoHelper& UpDirection(const MathLib::Vector3& up_direction);

        CameraDtoHelper& Frustum(const std::string& name, Frustum::ProjectionType projection_type);
        CameraDtoHelper& FrustumFrontBackZ(float front_z, float back_z);
        CameraDtoHelper& FrustumFov(float fov);
        CameraDtoHelper& FrustumNearPlaneDimension(float width, float height);

        CameraDto ToCameraDto();
        Engine::GenericDto ToGenericDto();

    protected:
        CameraDto m_cameraDto;
        FrustumDto m_frustumDto;
    };
}

#endif // SCENE_GRAPH_DTO_HELPER_H

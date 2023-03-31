/*********************************************************************
 * \file   CameraDtoMaker.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _CAMERA_DTO_MAKER_H
#define _CAMERA_DTO_MAKER_H

#include "SceneGraph/CameraFrustumDtos.h"

class CameraDtoMaker
{
public:
    static Enigma::SceneGraph::CameraDto MakeCameraDto();
};

#endif // _CAMERA_DTO_MAKER_H

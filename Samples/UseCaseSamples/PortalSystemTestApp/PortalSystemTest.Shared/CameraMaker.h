/*********************************************************************
 * \file   CameraMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef CAMERA_MAKER_H
#define CAMERA_MAKER_H

#include "SceneGraph/Camera.h"

class CameraMaker
{
public:
    static std::shared_ptr<Enigma::SceneGraph::Camera> makeCamera(const Enigma::SceneGraph::SpatialId& id);
};

#endif // CAMERA_MAKER_H

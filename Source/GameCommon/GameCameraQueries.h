/*********************************************************************
 * \file   GameCameraQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 *********************************************************************/
#ifndef GAME_CAMERA_QUERIES_H
#define GAME_CAMERA_QUERIES_H

#include "Frameworks/Query.h"
#include "SceneGraph/Camera.h"

namespace Enigma::GameCommon
{
    class QueryPrimaryCamera : public Frameworks::Query<std::shared_ptr<SceneGraph::Camera>>
    {
    public:
        QueryPrimaryCamera() = default;
    };
}

#endif // GAME_CAMERA_QUERIES_H

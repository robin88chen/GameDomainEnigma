/*********************************************************************
 * \file   SceneGraphFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_FILE_STORE_MAPPER_H
#define SCENE_GRAPH_FILE_STORE_MAPPER_H

#include "SceneGraph/SceneGraphStoreMapper.h"

namespace Enigma::FileStorage
{
    class SceneGraphFileStoreMapper : public SceneGraph::SceneGraphStoreMapper
    {
    public:
        SceneGraphFileStoreMapper();
        ~SceneGraphFileStoreMapper();
    };
}

#endif // SCENE_GRAPH_FILE_STORE_MAPPER_H

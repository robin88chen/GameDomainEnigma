/*****************************************************************
 * \file   SceneGraphMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef SCENE_GRAPH_MAKER_H
#define SCENE_GRAPH_MAKER_H

#include "SceneGraph/SpatialId.h"
#include "SceneGraph/SceneGraphStoreMapper.h"

class SceneGraphMaker
{
public:
    static void makeOneLevelQuadNode(const Enigma::SceneGraph::SpatialId& id, const std::shared_ptr<Enigma::SceneGraph::SceneGraphStoreMapper>& store_mapper);
};

#endif // SCENE_GRAPH_MAKER_H

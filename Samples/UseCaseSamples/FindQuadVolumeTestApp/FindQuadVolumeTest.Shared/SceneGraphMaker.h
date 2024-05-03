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
    static void makeThreeLevelQuadTree(const Enigma::SceneGraph::SpatialId& root_id, const Enigma::SceneGraph::SpatialId& level1_id1, const Enigma::SceneGraph::SpatialId& level1_id2, const Enigma::SceneGraph::SpatialId& level2_id1, const std::shared_ptr<Enigma::SceneGraph::SceneGraphStoreMapper>& store_mapper);
};

#endif // SCENE_GRAPH_MAKER_H

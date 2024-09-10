/*********************************************************************
 * \file   SceneGraphMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _SCENE_GRAPH_MAKER_H
#define _SCENE_GRAPH_MAKER_H

#include "GameEngine/GenericDto.h"
#include "SceneGraph/SpatialId.h"
#include "Primitives/PrimitiveId.h"
#include <vector>

class SceneGraphMaker
{
public:
    static void makePawn(const Enigma::SceneGraph::SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id);
    static Enigma::Engine::GenericDto makeHydratedVisibilityNode(const Enigma::SceneGraph::SpatialId& id, const Enigma::SceneGraph::SpatialId& parent_id, const std::vector<Enigma::SceneGraph::SpatialId>& children);

    static Enigma::Engine::GenericDto makeSceneGraph(const Enigma::SceneGraph::SpatialId& root_id, const Enigma::SceneGraph::SpatialId& visibility_node_id);
};

#endif // _SCENE_GRAPH_MAKER_H

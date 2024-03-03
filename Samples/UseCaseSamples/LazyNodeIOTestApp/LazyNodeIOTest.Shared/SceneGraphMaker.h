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
    static void makePawm(const Enigma::SceneGraph::SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id);
    static void makeStoredLazyNode(const Enigma::SceneGraph::SpatialId& id, const std::vector<Enigma::SceneGraph::SpatialId>& children);

    static std::vector<Enigma::Engine::GenericDto> MakeSceneGraphDtos(const std::string& lazy_node_name);
};

#endif // _SCENE_GRAPH_MAKER_H

/*********************************************************************
 * \file   SceneGraphMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _SCENE_GRAPH_MAKER_H
#define _SCENE_GRAPH_MAKER_H

#include "GameEngine/BoundingVolume.h"
#include "GameEngine/GenericDto.h"
#include "SceneGraph/SpatialId.h"
#include "Primitives/PrimitiveId.h"
#include <vector>

class SceneGraphMaker
{
public:
    static Enigma::Engine::GenericDto makePawm(const Enigma::SceneGraph::SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id, const Enigma::Engine::BoundingVolume& geometry_bound);
    static Enigma::Engine::GenericDto makeInsideZoneNode(const Enigma::SceneGraph::SpatialId& node_id, const Enigma::SceneGraph::SpatialId& portal_id, const std::vector<Enigma::SceneGraph::SpatialId>& children);
    static Enigma::Engine::GenericDto makeOutsideRegion(const Enigma::SceneGraph::SpatialId& outside_id, const Enigma::SceneGraph::SpatialId& root_id, const Enigma::SceneGraph::SpatialId& portal_id, const Enigma::SceneGraph::SpatialId& inside_zone_id, const std::vector<Enigma::SceneGraph::SpatialId>& children);
    static Enigma::Engine::GenericDto makeSceneGraph(const Enigma::SceneGraph::SpatialId& root_id, const Enigma::SceneGraph::SpatialId& outside_region_id, const Enigma::SceneGraph::SpatialId& portal_id, const Enigma::SceneGraph::SpatialId& inside_zone_id);
};

#endif // _SCENE_GRAPH_MAKER_H

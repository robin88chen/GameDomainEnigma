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
#include "SceneGraph/PortalZoneNodeAssembler.h"
#include "SceneGraph/OutRegionNodeAssembler.h"
#include <vector>

class SceneGraphMaker
{
public:
    static Enigma::Engine::GenericDto makePawn(const Enigma::SceneGraph::SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id, const Enigma::Engine::BoundingVolume& geometry_bound);
    static std::tuple<std::shared_ptr<Enigma::SceneGraph::HydratedPortalZoneNodeAssembler>, std::shared_ptr<Enigma::SceneGraph::DehydratedPortalZoneNodeAssembler>> makeInsideZoneNode(const Enigma::SceneGraph::SpatialId& node_id, const Enigma::SceneGraph::SpatialId& portal_id, const std::vector<Enigma::SceneGraph::SpatialId>& children);
    static std::tuple<std::shared_ptr<Enigma::SceneGraph::HydratedOutRegionNodeAssembler>, std::shared_ptr<Enigma::SceneGraph::DehydratedOutRegionNodeAssembler>> makeOutsideRegion(const Enigma::SceneGraph::SpatialId& outside_id, const Enigma::SceneGraph::SpatialId& root_id, const Enigma::SceneGraph::SpatialId& portal_id, const Enigma::SceneGraph::SpatialId& inside_zone_id, const std::vector<Enigma::SceneGraph::SpatialId>& children);
    static Enigma::Engine::GenericDto makePortalManagementNode(const Enigma::SceneGraph::SpatialId& root_id, const std::shared_ptr<Enigma::SceneGraph::HydratedOutRegionNodeAssembler>& outside_region, const std::shared_ptr<Enigma::SceneGraph::HydratedPortalZoneNodeAssembler>& inside_zone);
};

#endif // _SCENE_GRAPH_MAKER_H

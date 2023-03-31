/*********************************************************************
 * \file   SceneGraphMaker.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _SCENE_GRAPH_MAKER_H
#define _SCENE_GRAPH_MAKER_H
#include <string>
#include <vector>
#include "GameEngine/GenericDto.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "SceneGraph/PortalDtos.h"

class SceneGraphMaker
{
public:
    static std::vector<Enigma::Engine::GenericDto> MakeFloorDtos(const std::string& node_name);
    static void MakeSavedFloorNode(const std::string& node_name);
    static std::vector<Enigma::Engine::GenericDto> MakePortalSceneGraph(const std::string& node_name);
    static void MakeSavedPortalSceneGraph(const std::string& node_name);

    static Enigma::SceneGraph::PortalZoneNodeDto MakeOutsideRegion(const std::string name, const Enigma::MathLib::Matrix4& parent_world);

private:
    static Enigma::SceneGraph::PawnDto MakeFloorPawn(const Enigma::MathLib::Matrix4& parent_world);
    static Enigma::SceneGraph::PawnDto MakeDoorPawn(const Enigma::MathLib::Matrix4& parent_world);
    static Enigma::SceneGraph::PawnDto MakeBoardPawn(const Enigma::MathLib::Matrix4& parent_world);
    static Enigma::SceneGraph::PortalDto MakeDirectingInsidePortal(const Enigma::MathLib::Matrix4& parent_world);
    static Enigma::SceneGraph::PortalZoneNodeDto MakeInsideRegion(const std::string name, const Enigma::MathLib::Matrix4& parent_world);
    static void SaveLazyRegion(const std::string& node_name, const std::vector<Enigma::Engine::GenericDto>& dtos);
};

#endif // _SCENE_GRAPH_MAKER_H

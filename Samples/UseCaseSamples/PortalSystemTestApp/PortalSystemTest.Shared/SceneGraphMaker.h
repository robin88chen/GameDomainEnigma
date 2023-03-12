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
#include <GameEngine/GenericDto.h>

class SceneGraphMaker
{
public:
    static std::vector<Enigma::Engine::GenericDto> MakeFloorDtos(const std::string& node_name);
    static void MakeSavedFloorNode(const std::string& node_name);
};

#endif // _SCENE_GRAPH_MAKER_H

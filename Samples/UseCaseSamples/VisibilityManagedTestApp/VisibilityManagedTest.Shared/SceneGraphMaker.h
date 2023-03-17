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
#include <vector>

class SceneGraphMaker
{
public:
    static void MakeSavedVisibilityNode(const std::string& name);

    static std::vector<Enigma::Engine::GenericDto> MakeSceneGraphDtos(const std::string& visibility_node_name);
};

#endif // _SCENE_GRAPH_MAKER_H

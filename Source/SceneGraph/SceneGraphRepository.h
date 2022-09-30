/*********************************************************************
 * \file   SceneGraphRepository.h
 * \brief  scene graph objects repository, maintain objects lifecycle
 * and some global settings
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SCENE_GRAPH_REPOSITORY_H
#define SCENE_GRAPH_REPOSITORY_H

namespace Enigma::SceneGraph
{
    enum class GraphicCoordSys
    {
        LeftHand,
        RightHand
    };
    class SceneGraphRepository
    {
    public:
        static void SetCoordinateSystem(GraphicCoordSys hand);
        static GraphicCoordSys GetCoordinateSystem();
    private:
        static GraphicCoordSys m_handSystem;
    };
}

#endif // SCENE_GRAPH_REPOSITORY_H

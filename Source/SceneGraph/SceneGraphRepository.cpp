#include "SceneGraphRepository.h"

using namespace Enigma::SceneGraph;

GraphicCoordSys SceneGraphRepository::m_handSystem = GraphicCoordSys::LeftHand;

void SceneGraphRepository::SetCoordinateSystem(GraphicCoordSys hand)
{
    m_handSystem = hand;
}

GraphicCoordSys SceneGraphRepository::GetCoordinateSystem()
{
    return m_handSystem;
}

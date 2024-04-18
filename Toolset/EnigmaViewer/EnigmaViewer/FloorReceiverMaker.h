/*********************************************************************
 * \file   FloorReceiverMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 *********************************************************************/
#ifndef FLOOR_RECEIVER_MAKER_H
#define FLOOR_RECEIVER_MAKER_H

#include "SceneGraph/Node.h"

namespace EnigmaViewer
{
    class FloorReceiverMaker
    {
    public:
        static void makeFloorReceiver(const std::shared_ptr<Enigma::SceneGraph::Node>& root);
    };
}

#endif // FLOOR_RECEIVER_MAKER_H

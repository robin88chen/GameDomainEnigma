/*********************************************************************
 * \file   WorldMapQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef WORLD_MAP_QUERIES_H
#define WORLD_MAP_QUERIES_H

#include "Frameworks/Query.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/Node.h"
#include "MathLib/Vector3.h"
#include <memory>

namespace Enigma::WorldMap
{
    class QueryFittingNode : public Frameworks::Query<std::shared_ptr<SceneGraph::Node>>
    {
    public:
        QueryFittingNode(const Engine::BoundingVolume& bv) : m_bv(bv) {}

        const Engine::BoundingVolume& getBoundingVolume() const { return m_bv; }

    private:
        Engine::BoundingVolume m_bv;
    };
}

#endif // WORLD_MAP_QUERIES_H

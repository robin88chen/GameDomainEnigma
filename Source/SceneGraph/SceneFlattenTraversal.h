/*********************************************************************
 * \file   SceneFlattenTraversal.h
 * \brief  traverse scene graph to retrieve spatial, make a flatten array
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SCENE_FLATTEN_TRAVERSAL_H
#define SCENE_FLATTEN_TRAVERSAL_H

#include "SceneTraveler.h"
#include <vector>

namespace Enigma::SceneGraph
{
    class Spatial;

    class SceneFlattenTraversal : public SceneTraveler
    {
    public:
        SceneFlattenTraversal();
        SceneFlattenTraversal(const SceneFlattenTraversal&) = default;
        SceneFlattenTraversal(SceneFlattenTraversal&&) = default;
        ~SceneFlattenTraversal() override;
        SceneFlattenTraversal& operator=(const SceneFlattenTraversal&) = default;
        SceneFlattenTraversal& operator=(SceneFlattenTraversal&&) = default;

        virtual TravelResult travelTo(const SpatialPtr&) override;

        const std::vector<SpatialPtr>& GetSpatials() { return m_spatials; }

    protected:
        std::vector<SpatialPtr> m_spatials;
    };
}

#endif // SCENE_FLATTEN_TRAVERSAL_H

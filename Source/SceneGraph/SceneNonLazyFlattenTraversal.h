/*********************************************************************
 * \file   SceneNonLazyFlattenTraversal.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_NON_LAZY_FLATTEN_TRAVERSAL_H
#define SCENE_NON_LAZY_FLATTEN_TRAVERSAL_H

#include "SceneTraveler.h"
#include <vector>
#include <memory>

namespace Enigma::SceneGraph
{
    class Spatial;
    class SceneNonLazyFlattenTraversal : public SceneTraveler
    {
    public:
        SceneNonLazyFlattenTraversal();
        SceneNonLazyFlattenTraversal(const SceneNonLazyFlattenTraversal&) = default;
        SceneNonLazyFlattenTraversal(SceneNonLazyFlattenTraversal&&) = default;
        ~SceneNonLazyFlattenTraversal() override;
        SceneNonLazyFlattenTraversal& operator=(const SceneNonLazyFlattenTraversal&) = default;
        SceneNonLazyFlattenTraversal& operator=(SceneNonLazyFlattenTraversal&&) = default;

        virtual TravelResult travelTo(const SpatialPtr&) override;

        const std::vector<std::shared_ptr<Spatial>>& GetSpatials() { return m_spatials; }

    protected:
        std::vector<std::shared_ptr<Spatial>> m_spatials;
    };
}

#endif // SCENE_NON_LAZY_FLATTEN_TRAVERSAL_H

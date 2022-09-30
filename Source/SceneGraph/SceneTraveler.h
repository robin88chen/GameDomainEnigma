/*********************************************************************
 * \file   SceneTraveler.h
 * \brief  Scene Traveler, a service object
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef SCENE_TRAVELER_H
#define SCENE_TRAVELER_H

#include <memory>

namespace Enigma::SceneGraph
{
    class Spatial;
    using SpatialPtr = std::shared_ptr<Spatial>;

    /** Scene traveler */
    class SceneTraveler
    {
    public:
        enum class TravelResult
        {
            InterruptError = 0,  ///< error interrupt
            InterruptTargetFound,       ///< interrupt bcz target is found
            TestFail,
            Continue,
            Skip,
        };
    public:
        SceneTraveler() {};
        virtual ~SceneTraveler() {};

        virtual TravelResult TravelTo(const SpatialPtr&) = 0;
    };
};

#endif // SCENE_TRAVELER_H

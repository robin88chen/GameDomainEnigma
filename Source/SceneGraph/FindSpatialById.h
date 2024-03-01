/*********************************************************************
 * \file   FindSpatialById.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _FIND_SPATIAL_BY_ID_H
#define _FIND_SPATIAL_BY_ID_H

#include "SceneTraveler.h"
#include "Spatial.h"
#include <string>

namespace Enigma::SceneGraph
{
    class FindSpatialById : public SceneTraveler
    {
    public:
        FindSpatialById(const SpatialId& spatial_id);
        FindSpatialById(const FindSpatialById&) = default;
        FindSpatialById(FindSpatialById&&) = default;
        virtual ~FindSpatialById() override;
        FindSpatialById& operator=(const FindSpatialById&) = default;
        FindSpatialById& operator=(FindSpatialById&&) = default;

        virtual TravelResult travelTo(const SpatialPtr& spatial) override;

        SpatialPtr getFoundSpatial() { return m_foundSpatial; };

    private:
        SpatialId m_targetSpatialId;
        SpatialPtr m_foundSpatial;
    };
}

#endif // _FIND_SPATIAL_BY_ID_H

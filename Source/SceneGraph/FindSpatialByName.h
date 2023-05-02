/*********************************************************************
 * \file   FindSpatialByName.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _FIND_SPATIAL_BY_NAME_H
#define _FIND_SPATIAL_BY_NAME_H

#include "SceneTraveler.h"
#include <string>

namespace Enigma::SceneGraph
{
    class FindSpatialByName : public SceneTraveler
    {
    public:
        FindSpatialByName(const std::string& spatial_name);
        FindSpatialByName(const FindSpatialByName&) = default;
        FindSpatialByName(FindSpatialByName&&) = default;
        virtual ~FindSpatialByName() override;
        FindSpatialByName& operator=(const FindSpatialByName&) = default;
        FindSpatialByName& operator=(FindSpatialByName&&) = default;

        virtual TravelResult TravelTo(const SpatialPtr& spatial) override;

        SpatialPtr GetFoundSpatial() { return m_foundSpatial; };

    private:
        std::string m_targetSpatialName;
        SpatialPtr m_foundSpatial;
    };
}

#endif // _FIND_SPATIAL_BY_NAME_H

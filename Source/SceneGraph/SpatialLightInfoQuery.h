/*********************************************************************
 * \file   SpatialLightInfoQuery.h
 * \brief  Spatial Light Info Query, utility object, collecting
 *      light info by spatial attributes.
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef SPATIAL_LIGHT_INFO_QUERY_H
#define SPATIAL_LIGHT_INFO_QUERY_H

#include "LightInfo.h"
#include "MathLib/Vector3.h"
#include <list>

namespace Enigma::SceneGraph
{
    class SpatialLightInfoQuery
    {
    public:
        struct LightInfoDistance
        {
            LightInfo m_lightInfo;
            float m_sqrDistance;
            LightInfoDistance();
            LightInfoDistance(const LightInfoDistance& d);
            LightInfoDistance(const LightInfo& info, float sqrdist);
            ~LightInfoDistance();
        };

        typedef std::list<LightInfoDistance> LightInfoList;

    public:
        SpatialLightInfoQuery();
        SpatialLightInfoQuery(const SpatialLightInfoQuery&) = default;
        SpatialLightInfoQuery(SpatialLightInfoQuery&&) = default;
        ~SpatialLightInfoQuery();
        SpatialLightInfoQuery& operator=(const SpatialLightInfoQuery&) = default;
        SpatialLightInfoQuery& operator=(SpatialLightInfoQuery&&) = default;

        void InitSpatialPosition(const MathLib::Vector3& spatialPos) { m_spatialPos = spatialPos; };
        void Test(const LightInfo& info);

        const LightInfoList& GetResultList() { return m_listLightInfo; };
        unsigned int GetPointLightCount() const { return m_countPointLight; };

    protected:
        MathLib::Vector3 m_spatialPos;
        LightInfoList m_listLightInfo;
        unsigned int m_countPointLight;
    };
};


#endif // SPATIAL_LIGHT_INFO_QUERY_H

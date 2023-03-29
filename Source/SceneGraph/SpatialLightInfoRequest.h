/*********************************************************************
 * \file   SpatialLightInfoRequest.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _SPATIAL_LIGHT_INFO_REQUEST_H
#define _SPATIAL_LIGHT_INFO_REQUEST_H

#include "Frameworks/Request.h"
#include "MathLib/Vector3.h"

namespace Enigma::SceneGraph
{
    class RequestSpatialLightInfo : public Frameworks::IRequest
    {
    public:
        RequestSpatialLightInfo(const MathLib::Vector3& spatial_pos) : m_spatialPos(spatial_pos) {}
        const MathLib::Vector3& GetSpatialPos() const { return m_spatialPos; }

    private:
        MathLib::Vector3 m_spatialPos;
    };
}

#endif // _SPATIAL_LIGHT_INFO_REQUEST_H

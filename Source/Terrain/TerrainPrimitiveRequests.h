/*********************************************************************
 * \file   TerrainPrimitiveRequests.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_REQUESTS_H
#define TERRAIN_PRIMITIVE_REQUESTS_H

#include "Frameworks/Request.h"

namespace Enigma::Terrain
{
    class TerrainPrimitivePolicy;

    class RequestBuildTerrainPrimitive : public Frameworks::IRequest
    {
    public:
        RequestBuildTerrainPrimitive(const std::shared_ptr<TerrainPrimitivePolicy>& policy) : m_policy(policy) {}
        const std::shared_ptr<TerrainPrimitivePolicy>& GetPolicy() { return m_policy; }

    private:
        std::shared_ptr<TerrainPrimitivePolicy> m_policy;
    };
}


#endif // TERRAIN_PRIMITIVE_REQUESTS_H

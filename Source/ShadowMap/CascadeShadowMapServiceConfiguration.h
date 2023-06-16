/*********************************************************************
 * \file   CascadeShadowMapServiceConfiguration.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef CASCADE_SHADOWMAP_SERVICE_CONFIGURATION_H
#define CASCADE_SHADOWMAP_SERVICE_CONFIGURATION_H

#include "ShadowMapServiceConfiguration.h"
#include "MathLib/AlgebraBasicTypes.h"

namespace Enigma::ShadowMap
{
    class CascadeShadowMapServiceConfiguration : public ShadowMapServiceConfiguration
    {
    public:
        CascadeShadowMapServiceConfiguration();

        unsigned FrustaPartitionCount() const;
        unsigned& FrustaPartitionCount();

        const std::string& BackfaceCullingStateName() const;
        std::string& BackfaceCullingStateName();

    protected:
        unsigned m_frustaPartitionCount;
        std::string m_backfaceCullingStateName;
    };
}

#endif // CASCADE_SHADOWMAP_SERVICE_CONFIGURATION_H

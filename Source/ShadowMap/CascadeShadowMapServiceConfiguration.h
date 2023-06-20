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

        const std::string& CascadeDistanceSemantic() const;
        std::string& CascadeDistanceSemantic();
        const std::string& CascadeTextureCoordTransformSemantic() const;
        std::string& CascadeTextureCoordTransformSemantic();
        const std::string& CascadeSliceCountSemantic() const;
        std::string& CascadeSliceCountSemantic();

    protected:
        unsigned m_frustaPartitionCount;
        std::string m_backfaceCullingStateName;

        std::string m_cascadeDistanceSemantic;
        std::string m_cascadeTextureCoordTransformSemantic;
        std::string m_cascadeSliceCountSemantic;
    };
}

#endif // CASCADE_SHADOWMAP_SERVICE_CONFIGURATION_H

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

        unsigned frustaPartitionCount() const;
        unsigned& frustaPartitionCount();

        const std::string& backfaceCullingStateName() const;
        std::string& backfaceCullingStateName();

        const std::string& cascadeDistanceSemantic() const;
        std::string& cascadeDistanceSemantic();
        const std::string& cascadeTextureCoordTransformSemantic() const;
        std::string& cascadeTextureCoordTransformSemantic();
        const std::string& cascadeSliceCountSemantic() const;
        std::string& cascadeSliceCountSemantic();

    protected:
        unsigned m_frustaPartitionCount;
        std::string m_backfaceCullingStateName;

        std::string m_cascadeDistanceSemantic;
        std::string m_cascadeTextureCoordTransformSemantic;
        std::string m_cascadeSliceCountSemantic;
    };
}

#endif // CASCADE_SHADOWMAP_SERVICE_CONFIGURATION_H

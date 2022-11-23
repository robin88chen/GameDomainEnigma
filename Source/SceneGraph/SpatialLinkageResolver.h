/*********************************************************************
 * \file   SpatialLinkageResolver.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _SPATIAL_LINKAGE_RESOLVER_H
#define _SPATIAL_LINKAGE_RESOLVER_H

#include "GameEngine/LinkageResolver.h"
#include "Frameworks/EventSubscriber.h"

namespace Enigma::SceneGraph
{
    class Spatial;

    class SpatialLinkageResolver : public Engine::FactoryLinkageResolver<Spatial>
    {
    public:
        SpatialLinkageResolver();
        SpatialLinkageResolver(const Engine::FactoryQuery<Spatial>& query);
        SpatialLinkageResolver(const SpatialLinkageResolver&);
        SpatialLinkageResolver(SpatialLinkageResolver&&) = delete;
        ~SpatialLinkageResolver();
        SpatialLinkageResolver& operator=(const SpatialLinkageResolver&);
        SpatialLinkageResolver& operator=(SpatialLinkageResolver&&) = delete;

        void ClearResolvers();

    protected:
        void OnFactorySpatialCreated(const Frameworks::IEventPtr& e);

    protected:
        Frameworks::EventSubscriberPtr m_onFactorySpatialCreated;
    };
}

#endif // _SPATIAL_LINKAGE_RESOLVER_H

/*********************************************************************
 * \file   LightInfoTraversal.h
 * \brief  Light Info Manager, maintain & query service
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef LIGHT_INFO_TRAVERSAL_H
#define LIGHT_INFO_TRAVERSAL_H

#include "Frameworks/SystemService.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/QuerySubscriber.h"
#include "SpatialId.h"
#include "SpatialRenderState.h"
#include <system_error>
#include <unordered_map>
#include <mutex>
#include <deque>

namespace Enigma::SceneGraph
{
    using error = std::error_code;

    class SpatialLightInfoQuery;
    class Light;

    class LightInfoTraversal : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        LightInfoTraversal(Frameworks::ServiceManager* srv_mngr);
        LightInfoTraversal(const LightInfoTraversal&) = delete;
        LightInfoTraversal(LightInfoTraversal&&) = delete;
        ~LightInfoTraversal() override;
        LightInfoTraversal& operator=(const LightInfoTraversal&) = delete;
        LightInfoTraversal& operator=(LightInfoTraversal&&) = delete;

    protected:
        SpatialRenderState queryLightingStateAt(const MathLib::Vector3& wolrd_position);

        void onLightInfoCreated(const Frameworks::IEventPtr& e);
        void onLightInfoDeleted(const Frameworks::IEventPtr& e);

        void queryLightingStateAt(const Frameworks::IQueryPtr& q);

    protected:
        Frameworks::EventSubscriberPtr m_onLightInfoCreated;
        Frameworks::EventSubscriberPtr m_onLightInfoDeleted;
        Frameworks::QuerySubscriberPtr m_queryLightingStateAt;
        typedef std::unordered_map<SpatialId, std::weak_ptr<Light>, SpatialId::hash> LightNodeMap;
        LightNodeMap m_lights;
        std::recursive_mutex m_mapLock;
    };
};

#endif // LIGHT_INFO_TRAVERSAL_H

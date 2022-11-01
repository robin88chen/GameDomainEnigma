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
#include <system_error>
#include <unordered_map>
#include <mutex>

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

        void QuerySpatialLightInfo(SpatialLightInfoQuery& query);

    protected:
        void OnLightInfoCreated(const Frameworks::IEventPtr& e);
        void OnLightInfoDeleted(const Frameworks::IEventPtr& e);

    protected:
        Frameworks::EventSubscriberPtr m_onLightInfoCreated;
        Frameworks::EventSubscriberPtr m_onLightInfoDeleted;

        typedef std::unordered_map<std::string, std::weak_ptr<Light>> LightNodeMap;
        LightNodeMap m_lights;
        std::recursive_mutex m_mapLock;
    };
};

#endif // LIGHT_INFO_TRAVERSAL_H

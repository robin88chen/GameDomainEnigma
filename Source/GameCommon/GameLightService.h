/*********************************************************************
 * \file   GameLightService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef GAME_LIGHT_SERVICE_H
#define GAME_LIGHT_SERVICE_H

#include "Frameworks/SystemService.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/Node.h"
#include <unordered_set>

namespace Enigma::SceneGraph
{
    class Light;
    class LightInfo;
}
namespace Enigma::GameCommon
{
    class GameLightService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GameLightService(Frameworks::ServiceManager* mngr);
        GameLightService(const GameLightService&) = delete;
        GameLightService(GameLightService&&) = delete;
        virtual ~GameLightService() override = default;
        GameLightService& operator=(const GameLightService&) = delete;
        GameLightService& operator=(GameLightService&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        void createAmbientLight(const SceneGraph::SpatialId& parent_id, const SceneGraph::SpatialId& light_id, const SceneGraph::LightInfo& info, const Engine::FactoryDesc& fd, SceneGraph::PersistenceLevel persistence_level);
        void createSunLight(const SceneGraph::SpatialId& parent_id, const SceneGraph::SpatialId& light_id, const SceneGraph::LightInfo& info, const Engine::FactoryDesc& fd, SceneGraph::PersistenceLevel persistence_level);
        void createPointLight(const SceneGraph::SpatialId& parent_id, const MathLib::Matrix4& mxLocal, const SceneGraph::SpatialId& light_id, const SceneGraph::LightInfo& info, const Engine::FactoryDesc& fd, SceneGraph::PersistenceLevel persistence_level);

    protected:
        void createAmbientLight(const Frameworks::ICommandPtr& command);
        void createSunLight(const Frameworks::ICommandPtr& command);
        void createPointLight(const Frameworks::ICommandPtr& command);
        void changeLightColor(const Frameworks::ICommandPtr& command) const;
        void changeLightDirection(const Frameworks::ICommandPtr& command) const;
        void changeLightPosition(const Frameworks::ICommandPtr& command) const;
        void changeLightAttenuation(const Frameworks::ICommandPtr& command) const;
        void changeLightRange(const Frameworks::ICommandPtr& command) const;
        void changeLightAbility(const Frameworks::ICommandPtr& command) const;
        void deleteLight(const Frameworks::ICommandPtr& command) const;

        void onSceneNodeChildAttached(const Frameworks::IEventPtr& event);
        void onAttachSceneNodeChildFailed(const Frameworks::IEventPtr& event);

    protected:
        std::unordered_set<SceneGraph::SpatialId, SceneGraph::SpatialId::hash> m_pendingLightIds;

        Frameworks::CommandSubscriberPtr m_createAmbientLight;
        Frameworks::CommandSubscriberPtr m_createSunLight;
        Frameworks::CommandSubscriberPtr m_createPointLight;
        Frameworks::CommandSubscriberPtr m_changeLightColor;
        Frameworks::CommandSubscriberPtr m_changeLightDirection;
        Frameworks::CommandSubscriberPtr m_changeLightPosition;
        Frameworks::CommandSubscriberPtr m_changeLightAttenuation;
        Frameworks::CommandSubscriberPtr m_changeLightRange;
        Frameworks::CommandSubscriberPtr m_changeLightEnable;
        Frameworks::CommandSubscriberPtr m_changeLightDisable;
        Frameworks::CommandSubscriberPtr m_deleteLight;

        Frameworks::EventSubscriberPtr m_onSceneNodeChildAttached;
        Frameworks::EventSubscriberPtr m_onAttachSceneNodeChildFailed;
    };
}

#endif // GAME_LIGHT_SERVICE_H

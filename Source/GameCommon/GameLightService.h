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

namespace Enigma::GameCommon
{
    class GameLightService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GameLightService(Frameworks::ServiceManager* mngr, const std::shared_ptr<SceneGraph::SceneGraphRepository>& scene_graph_repository);
        GameLightService(const GameLightService&) = delete;
        GameLightService(GameLightService&&) = delete;
        virtual ~GameLightService() override = default;
        GameLightService& operator=(const GameLightService&) = delete;
        GameLightService& operator=(GameLightService&&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        void CreateAmbientLight(const std::string& parent_name, const std::string& lightName, const MathLib::ColorRGBA& colorLight);
        void CreateSunLight(const std::string& parent_name, const std::string& lightName, const MathLib::Vector3& dirLight, const MathLib::ColorRGBA& colorLight);

        void CreatePointLight(const std::string& parent_name, const MathLib::Matrix4& mxLocal, const std::string& lightName,
            const MathLib::Vector3& vecPos, const MathLib::ColorRGBA& color, float range);

    protected:
        void DoCreatingAmbientLight(const Frameworks::ICommandPtr& command);
        void DoCreatingSunLight(const Frameworks::ICommandPtr& command);
        void DoCreatingPointLight(const Frameworks::ICommandPtr& command);
        void DoChangingLightColor(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightDirection(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightPosition(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightAttenuation(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightRange(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightAbility(const Frameworks::ICommandPtr& command) const;
        void DoDeletingLight(const Frameworks::ICommandPtr& command) const;

        void OnSceneNodeChildAttached(const Frameworks::IEventPtr& event);
        void OnAttachSceneNodeChildFailed(const Frameworks::IEventPtr& event);

    protected:
        std::weak_ptr<SceneGraph::SceneGraphRepository> m_sceneGraphRepository;

        std::unordered_set<std::string> m_pendingLightNames;

        Frameworks::CommandSubscriberPtr m_doCreatingAmbientLight;
        Frameworks::CommandSubscriberPtr m_doCreatingSunLight;
        Frameworks::CommandSubscriberPtr m_doCreatingPointLight;
        Frameworks::CommandSubscriberPtr m_doChangingLightColor;
        Frameworks::CommandSubscriberPtr m_doChangingLightDirection;
        Frameworks::CommandSubscriberPtr m_doChangingLightPosition;
        Frameworks::CommandSubscriberPtr m_doChangingLightAttenuation;
        Frameworks::CommandSubscriberPtr m_doChangingLightRange;
        Frameworks::CommandSubscriberPtr m_doChangingLightEnable;
        Frameworks::CommandSubscriberPtr m_doChangingLightDisable;
        Frameworks::CommandSubscriberPtr m_doDeletingLight;

        Frameworks::EventSubscriberPtr m_onSceneNodeChildAttached;
        Frameworks::EventSubscriberPtr m_onAttachSceneNodeChildFailed;
    };
}

#endif // GAME_LIGHT_SERVICE_H

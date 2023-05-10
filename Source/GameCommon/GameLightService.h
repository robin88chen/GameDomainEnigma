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
#include "SceneGraph/Node.h"

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

        void CreateAmbientLight(const std::shared_ptr<SceneGraph::Node>& parent,
            const std::string& lightName, const MathLib::ColorRGBA& colorLight) const;
        void CreateSunLight(const std::shared_ptr<SceneGraph::Node>& parent,
            const std::string& lightName, const MathLib::Vector3& dirLight, const MathLib::ColorRGBA& colorLight) const;

        void CreatePointLight(const std::shared_ptr<SceneGraph::Node>& parent, const MathLib::Matrix4& mxLocal,
            const std::string& lightName, const MathLib::Vector3& vecPos, const MathLib::ColorRGBA& color) const;

    protected:
        void DoCreatingAmbientLight(const Frameworks::ICommandPtr& command) const;
        void DoCreatingSunLight(const Frameworks::ICommandPtr& command) const;
        void DoCreatingPointLight(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightColor(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightDirection(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightPosition(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightAttenuation(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightRange(const Frameworks::ICommandPtr& command) const;
        void DoChangingLightAbility(const Frameworks::ICommandPtr& command) const;

    protected:
        std::weak_ptr<SceneGraph::SceneGraphRepository> m_sceneGraphRepository;

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
    };
}

#endif // GAME_LIGHT_SERVICE_H

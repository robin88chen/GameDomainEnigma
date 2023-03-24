/*********************************************************************
 * \file   InstallingPolicies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef INSTALLING_POLICIES_H
#define INSTALLING_POLICIES_H

#include "GraphicKernel/IGraphicAPI.h"
#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/EffectCompilingProfileDeserializer.h"
#include "InputHandlers/InputHandlerService.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "Frameworks/EventSubscriber.h"
#include "Renderer/RendererManager.h"
#include <memory>
#include <system_error>

namespace Enigma::Controllers
{
    using ServiceCreator = std::function<Frameworks::ISystemService*(Frameworks::ServiceManager*)>;
    using error = std::error_code;

    class InstallingPolicy
    {
    public:
        InstallingPolicy() = default;
        virtual ~InstallingPolicy() = default;
    };
    class DeviceCreatingPolicy : public InstallingPolicy
    {
    public:
        DeviceCreatingPolicy(const Graphics::DeviceRequiredBits& rqb, void* hwnd);

        const Graphics::DeviceRequiredBits& RequiredBits() { return m_rqb; }
        void* Hwnd() { return m_hwnd; }
    protected:
        Graphics::DeviceRequiredBits m_rqb;
        void* m_hwnd;
    };
    class RendererInstallingPolicy : public InstallingPolicy
    {
    public:
        RendererInstallingPolicy() = default;

        virtual error WhenInstalling(const std::shared_ptr<Renderer::RendererManager>& manager) = 0;
        virtual error WhenShutdown(const std::shared_ptr<Renderer::RendererManager>& manager) = 0;
    };

    class DefaultRendererInstallingPolicy : public RendererInstallingPolicy
    {
    public:
        DefaultRendererInstallingPolicy(const std::string& renderer_name, const std::string& primary_target_name);

        const std::string& GetRendererName() { return m_rendererName; }
        const std::string& GetPrimaryTargetName() { return m_primaryTargetName; }

        error WhenInstalling(const std::shared_ptr<Renderer::RendererManager>& manager) override;
        error WhenShutdown(const std::shared_ptr<Renderer::RendererManager>& manager) override;

    protected:
        std::string m_rendererName;
        std::string m_primaryTargetName;
    };
    class SceneGraphBuildingPolicy : public InstallingPolicy
    {
    public:
        SceneGraphBuildingPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer,
            const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer);

        const std::shared_ptr<Engine::IDtoDeserializer>& GetDtoDeserializer() { return m_dtoDeserializer; }
        const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& GetEffectDeserializer() { return m_effectDeserializer; }

    protected:
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;
        std::shared_ptr<Engine::IEffectCompilingProfileDeserializer> m_effectDeserializer;
    };
    class InputHandlerInstallingPolicy : public InstallingPolicy
    {
    public:
        InputHandlerInstallingPolicy(const ServiceCreator& creator);

        std::shared_ptr<Frameworks::ISystemService> CreateService(Frameworks::ServiceManager* manager);
        const std::shared_ptr<InputHandlers::InputHandlerService>& GetInputHandler() { return m_inputHandler; }

    protected:
        ServiceCreator m_creator;
        std::shared_ptr<InputHandlers::InputHandlerService> m_inputHandler;
    };

    class InstallingPolicyGroup
    {
    public:
        InstallingPolicyGroup() = default;
        InstallingPolicyGroup(const std::vector<std::shared_ptr<InstallingPolicy>>& policies) : m_policies(policies) {};

        std::shared_ptr<DeviceCreatingPolicy> FindDeviceCreatingPolicy();
        std::shared_ptr<RendererInstallingPolicy> FindRendererInstallingPolicy();
        std::shared_ptr<SceneGraphBuildingPolicy> FindSceneGraphBuildingPolicy();
        std::shared_ptr<InputHandlerInstallingPolicy> FindInputHandlerInstallingPolicy();

    private:
        std::vector<std::shared_ptr<InstallingPolicy>> m_policies;
    };
}

#endif // INSTALLING_POLICIES_H

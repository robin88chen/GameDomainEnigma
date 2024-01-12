/*********************************************************************
 * \file   RendererInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _RENDERER_INSTALLING_POLICY_H
#define _RENDERER_INSTALLING_POLICY_H

#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/InstallingPolicy.h"

namespace Enigma::Renderer
{
    using error = std::error_code;

    class RenderSystemInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        RenderSystemInstallingPolicy() {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;
    };

    class RendererInstallingPolicy : public Engine::InstallingPolicy
    {
    protected:
        RendererInstallingPolicy() = default;
    };

    class DefaultRendererInstallingPolicy : public RendererInstallingPolicy
    {
    public:
        DefaultRendererInstallingPolicy(const std::string& renderer_name, const std::string& primary_target_name)
            : m_rendererName(renderer_name), m_primaryTargetName(primary_target_name) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::string m_rendererName;
        std::string m_primaryTargetName;
    };
}

#endif // _RENDERER_INSTALLING_POLICY_H

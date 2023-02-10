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
#include <memory>

namespace Enigma::Controllers
{
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
    class InstallingDefaultRendererPolicy : public InstallingPolicy
    {
    public:
        InstallingDefaultRendererPolicy(const std::string& renderer_name, const std::string& primary_target_name);

        const std::string& GetRendererName() { return m_rendererName; }
        const std::string& GetPrimaryTargetName() { return m_primaryTargetName; }

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
}

#endif // INSTALLING_POLICIES_H

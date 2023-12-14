/*********************************************************************
 * \file   EngineInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _ENGINE_INSTALLING_POLICY_H
#define _ENGINE_INSTALLING_POLICY_H

#include "InstallingPolicy.h"

namespace Enigma::Engine
{
    class IEffectCompilingProfileDeserializer;

    class EngineInstallingPolicy : public InstallingPolicy
    {
    public:
        EngineInstallingPolicy(const std::shared_ptr<IEffectCompilingProfileDeserializer>& effect_deserializer)
            : m_effectDeserializer(effect_deserializer) {};

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<Engine::IEffectCompilingProfileDeserializer> m_effectDeserializer;
    };
}
#endif // _ENGINE_INSTALLING_POLICY_H

/*********************************************************************
 * \file   PrefabInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PREFAB_INSTALLING_POLICY_H
#define PREFAB_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"
#include "GameEngine/DtoDeserializer.h"

namespace Enigma::Prefabs
{
    using error = std::error_code;

    class PrefabInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        PrefabInstallingPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer)
            : m_dtoDeserializer(dto_deserializer) {}

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;
    };
}

#endif // PREFAB_INSTALLING_POLICY_H

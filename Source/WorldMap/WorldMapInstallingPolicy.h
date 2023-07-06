/*********************************************************************
 * \file   WorldMapInstallingPolicy.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_INSTALLING_POLICY_H
#define WORLD_MAP_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"
#include <system_error>

namespace Enigma::WorldMap
{
    using error = std::error_code;

    class WorldMapInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        WorldMapInstallingPolicy() = default;

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;
    };
}

#endif // WORLD_MAP_INSTALLING_POLICY_H

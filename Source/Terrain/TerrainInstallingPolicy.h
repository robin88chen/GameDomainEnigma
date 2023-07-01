/*********************************************************************
 * \file   TerrainInstallingPolicy.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_INSTALLING_POLICY_H
#define TERRAIN_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"
#include <system_error>

namespace Enigma::Terrain
{
    using error = std::error_code;

    class TerrainInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        TerrainInstallingPolicy() = default;

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;
    };
}

#endif // TERRAIN_INSTALLING_POLICY_H

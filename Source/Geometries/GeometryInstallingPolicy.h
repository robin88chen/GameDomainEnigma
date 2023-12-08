/*********************************************************************
 * \file   GeometryInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_INSTALLING_POLICY_H
#define GEOMETRY_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"

namespace Enigma::Geometries
{
    using error = std::error_code;

    class GeometryDataStoreMapper;

    class GeometryInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        GeometryInstallingPolicy(const std::shared_ptr<GeometryDataStoreMapper>& store_mapper) : m_storeMapper(store_mapper) {};

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<GeometryDataStoreMapper> m_storeMapper;
    };
}

#endif // GEOMETRY_INSTALLING_POLICY_H

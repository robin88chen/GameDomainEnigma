/*********************************************************************
 * \file   PrimitiveRepositoryInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef PRIMITIVE_REPOSITORY_INSTALLING_POLICY_H
#define PRIMITIVE_REPOSITORY_INSTALLING_POLICY_H

#include "InstallingPolicy.h"

namespace Enigma::Engine
{
    class PrimitiveStoreMapper;

    class PrimitiveRepositoryInstallingPolicy : public InstallingPolicy
    {
    public:
        PrimitiveRepositoryInstallingPolicy(const std::shared_ptr<PrimitiveStoreMapper>& store_mapper) : m_storeMapper(store_mapper) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<PrimitiveStoreMapper> m_storeMapper;
    };
}

#endif // PRIMITIVE_REPOSITORY_INSTALLING_POLICY_H

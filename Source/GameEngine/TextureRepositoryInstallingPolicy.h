/*********************************************************************
 * \file   TextureRepositoryInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef TEXTURE_REPOSITORY_INSTALLING_POLICY_H
#define TEXTURE_REPOSITORY_INSTALLING_POLICY_H

#include "InstallingPolicy.h"

namespace Enigma::Engine
{
    class TextureStoreMapper;

    class TextureRepositoryInstallingPolicy : public InstallingPolicy
    {
    public:
        TextureRepositoryInstallingPolicy(const std::shared_ptr<TextureStoreMapper>& store_mapper) : m_storeMapper(store_mapper) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<TextureStoreMapper> m_storeMapper;
    };
}


#endif // TEXTURE_REPOSITORY_INSTALLING_POLICY_H

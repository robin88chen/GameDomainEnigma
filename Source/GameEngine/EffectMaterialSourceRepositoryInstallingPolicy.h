/*********************************************************************
 * \file   EffectMaterialSourceRepositoryInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef EFFECT_MATERIAL_SOURCE_REPOSITORY_INSTALLING_POLICY_H
#define EFFECT_MATERIAL_SOURCE_REPOSITORY_INSTALLING_POLICY_H

#include "InstallingPolicy.h"

namespace Enigma::Engine
{
    class EffectMaterialSourceStoreMapper;

    class EffectMaterialSourceRepositoryInstallingPolicy : public InstallingPolicy
    {
    public:
        EffectMaterialSourceRepositoryInstallingPolicy(const std::shared_ptr<EffectMaterialSourceStoreMapper>& store_mapper) : m_storeMapper(store_mapper) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<EffectMaterialSourceStoreMapper> m_storeMapper;
    };
}

#endif // EFFECT_MATERIAL_SOURCE_REPOSITORY_INSTALLING_POLICY_H

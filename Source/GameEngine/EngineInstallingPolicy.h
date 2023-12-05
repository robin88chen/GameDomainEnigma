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
    class GeometryDataStoreMapper;

    class EngineInstallingPolicy : public InstallingPolicy
    {
    public:
        EngineInstallingPolicy(const std::shared_ptr<IEffectCompilingProfileDeserializer>& effect_deserializer, const std::shared_ptr<GeometryDataStoreMapper>& store_mapper)
            : m_effectDeserializer(effect_deserializer), m_storeMapper(store_mapper) {};

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<Engine::IEffectCompilingProfileDeserializer> m_effectDeserializer;
        std::shared_ptr<Engine::GeometryDataStoreMapper> m_storeMapper;
    };
}
#endif // _ENGINE_INSTALLING_POLICY_H

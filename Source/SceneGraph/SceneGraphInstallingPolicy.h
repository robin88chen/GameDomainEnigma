/*********************************************************************
 * \file   SceneGraphInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _SCENE_GRAPH_INSTALLING_POLICY_H
#define _SCENE_GRAPH_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"
#include "GameEngine/DtoDeserializer.h"

namespace Enigma::SceneGraph
{
    using error = std::error_code;

    class SceneGraphStoreMapper;

    class SceneGraphInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        SceneGraphInstallingPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer, const std::shared_ptr<SceneGraphStoreMapper>& store_mapper)
            : m_dtoDeserializer(dto_deserializer), m_storeMapper(store_mapper) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;
        std::shared_ptr<SceneGraphStoreMapper> m_storeMapper;
    };
}

#endif // _SCENE_GRAPH_INSTALLING_POLICY_H

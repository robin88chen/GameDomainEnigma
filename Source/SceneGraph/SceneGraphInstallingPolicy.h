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

    class SceneGraphInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        SceneGraphInstallingPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer)
            : m_dtoDeserializer(dto_deserializer) {}

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;
    };
}

#endif // _SCENE_GRAPH_INSTALLING_POLICY_H

/*********************************************************************
 * \file   InputHandlerInstallingPolicy.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _INPUT_HANDLER_INSTALLING_POLICY_H
#define _INPUT_HANDLER_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"

namespace Enigma::InputHandlers
{
    using error = std::error_code;

    class InputHandlerService;
    class InputHandlerInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        InputHandlerInstallingPolicy() = default;
        virtual ~InputHandlerInstallingPolicy() override = default;
        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

        const std::shared_ptr<InputHandlerService>& GetInputHandler() { return m_inputHandler; }

    protected:
        std::shared_ptr<InputHandlerService> m_inputHandler;
    };
}

#endif // _INPUT_HANDLER_INSTALLING_POLICY_H

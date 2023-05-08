/*********************************************************************
 * \file   InstallingPolicy.h
 * \brief Installing Policy base interface
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _INSTALLING_POLICY_H
#define _INSTALLING_POLICY_H

#include "Frameworks/ServiceManager.h"
#include <system_error>

namespace Enigma::Engine
{
    using error = std::error_code;

    class InstallingPolicy
    {
    public:
        InstallingPolicy() = default;
        InstallingPolicy(const InstallingPolicy&) = delete;
        InstallingPolicy(InstallingPolicy&&) = delete;
        virtual ~InstallingPolicy() = default;
        InstallingPolicy& operator=(const InstallingPolicy&) = delete;
        InstallingPolicy& operator=(InstallingPolicy&&) = delete;

        virtual error Install(Frameworks::ServiceManager* service_manager) = 0;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) = 0;
    };
}

#endif // _INSTALLING_POLICY_H

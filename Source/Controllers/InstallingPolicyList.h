/*********************************************************************
 * \file   InstallingPolicyList.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _INSTALLING_POLICY_LIST_H
#define _INSTALLING_POLICY_LIST_H

#include "GameEngine/InstallingPolicy.h"
#include <list>

namespace Enigma::Controllers
{
    class InstallingPolicyList : public std::list<std::shared_ptr<Engine::InstallingPolicy>>
    {
    public:
        InstallingPolicyList(std::initializer_list<std::shared_ptr<Engine::InstallingPolicy>> initializer_list);
        
    };
}

#endif // _INSTALLING_POLICY_LIST_H

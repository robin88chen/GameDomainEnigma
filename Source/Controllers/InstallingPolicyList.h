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
#include <typeindex>

namespace Enigma::Controllers
{
    class InstallingPolicyList : public std::list<std::shared_ptr<Engine::InstallingPolicy>>
    {
    public:
        InstallingPolicyList() = default;
        InstallingPolicyList(std::initializer_list<std::shared_ptr<Engine::InstallingPolicy>> initializer_list);

        void SortOrder();
    private:
        void MakeOrderMap();

    private:
        std::unordered_map<std::type_index, std::uint16_t> m_orderMap;
    };
}

#endif // _INSTALLING_POLICY_LIST_H

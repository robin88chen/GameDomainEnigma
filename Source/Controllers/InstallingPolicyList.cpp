#include "InstallingPolicyList.h"

using namespace Enigma::Controllers;

InstallingPolicyList::InstallingPolicyList(std::initializer_list<std::shared_ptr<Engine::InstallingPolicy>> initializer_list)
    : std::list<std::shared_ptr<Engine::InstallingPolicy>>(initializer_list)
{

}


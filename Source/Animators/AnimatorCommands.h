/*********************************************************************
 * \file   AnimatorCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATOR_COMMANDS_H
#define _ANIMATOR_COMMANDS_H

#include "Frameworks/Command.h"

namespace Enigma::Animators
{
    class ModelAnimatorPolicy;

    class BuildModelAnimator : public Frameworks::ICommand
    {
    public:
        BuildModelAnimator(const std::shared_ptr<ModelAnimatorPolicy>& policy) : m_policy(policy) {}
        const std::shared_ptr<ModelAnimatorPolicy>& GetPolicy() { return m_policy; }

    private:
        std::shared_ptr<ModelAnimatorPolicy> m_policy;
    };
}

#endif // _ANIMATOR_COMMANDS_H

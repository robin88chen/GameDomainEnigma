/*********************************************************************
 * \file   TextureCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_COMMANDS_H
#define TEXTURE_COMMANDS_H

#include "Frameworks/Command.h"
#include "TextureLoadingPolicies.h"

namespace Enigma::Engine
{
    class LoadTexture : public Frameworks::ICommand
    {
    public:
        LoadTexture(const TexturePolicy& policy) : m_policy(policy) {}
        const TexturePolicy& GetPolicy() { return m_policy; }

    private:
        TexturePolicy m_policy;
    };
}

#endif // TEXTURE_COMMANDS_H

/*********************************************************************
 * \file   RenderBufferCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef RENDER_BUFFER_COMMANDS_H
#define RENDER_BUFFER_COMMANDS_H

#include "Frameworks/Command.h"
#include "RenderBufferBuildingPolicies.h"

namespace Enigma::Engine
{
    class BuildRenderBuffer : public Frameworks::ICommand
    {
    public:
        BuildRenderBuffer(const RenderBufferPolicy& policy) : m_policy(policy) {}
        const RenderBufferPolicy& GetPolicy() { return m_policy; }

    private:
        RenderBufferPolicy m_policy;
    };
}

#endif // RENDER_BUFFER_COMMANDS_H

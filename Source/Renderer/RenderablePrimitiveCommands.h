/*********************************************************************
 * \file   RenderablePrimitiveCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_COMMANDS_H
#define _RENDERABLE_PRIMITIVE_COMMANDS_H

#include "Frameworks/Command.h"
#include "RenderablePrimitivePolicies.h"

namespace Enigma::Renderer
{
    class BuildRenderablePrimitive : public Frameworks::ICommand
    {
    public:
        BuildRenderablePrimitive(const std::shared_ptr<RenderablePrimitivePolicy>& policy) : m_policy(policy) {}
        const std::shared_ptr<RenderablePrimitivePolicy>& GetPolicy() { return m_policy; }

    private:
        std::shared_ptr<RenderablePrimitivePolicy> m_policy;
    };
}

#endif // _RENDERABLE_PRIMITIVE_COMMANDS_H

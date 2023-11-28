/*********************************************************************
 * \file   RenderableCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef RENDERABLE_COMMANDS_H
#define RENDERABLE_COMMANDS_H

#include "Frameworks/Command.h"
#include "RenderablePrimitivePolicies.h"

namespace Enigma::Renderer
{
    class BuildRenderablePrimitive : public Frameworks::IRequestCommand
    {
    public:
        BuildRenderablePrimitive(const std::shared_ptr<RenderablePrimitivePolicy>& policy) : m_policy(policy) {}
        const std::shared_ptr<RenderablePrimitivePolicy>& getPolicy() { return m_policy; }

    private:
        std::shared_ptr<RenderablePrimitivePolicy> m_policy;
    };
}

#endif // RENDERABLE_COMMANDS_H

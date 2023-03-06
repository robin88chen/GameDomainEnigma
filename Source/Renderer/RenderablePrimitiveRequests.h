/*********************************************************************
 * \file   RenderablePrimitiveRequests.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_REQUESTS_H
#define _RENDERABLE_PRIMITIVE_REQUESTS_H

#include "Frameworks/Request.h"
#include "RenderablePrimitivePolicies.h"

namespace Enigma::Renderer
{
    class RequestBuildRenderablePrimitive : public Frameworks::IRequest
    {
    public:
        RequestBuildRenderablePrimitive(const std::shared_ptr<RenderablePrimitivePolicy>& policy) : m_policy(policy) {}
        const std::shared_ptr<RenderablePrimitivePolicy>& GetPolicy() { return m_policy; }

    private:
        std::shared_ptr<RenderablePrimitivePolicy> m_policy;
    };
}

#endif // _RENDERABLE_PRIMITIVE_REQUESTS_H

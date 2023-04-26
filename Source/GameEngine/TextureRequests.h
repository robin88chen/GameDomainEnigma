/*********************************************************************
 * \file   TextureRequests.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _TEXTURE_REQUESTS_H
#define _TEXTURE_REQUESTS_H

#include "TextureLoadingPolicies.h"
#include "Frameworks/Request.h"

namespace Enigma::Engine
{
    class RequestLoadTexture : public Frameworks::IRequest
    {
    public:
        RequestLoadTexture(const TexturePolicy& policy) : m_policy(policy) {}
        const TexturePolicy& GetPolicy() { return m_policy; }

    private:
        TexturePolicy m_policy;
    };
}

#endif // _TEXTURE_REQUESTS_H

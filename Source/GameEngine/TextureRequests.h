/*********************************************************************
 * \file   TextureRequests.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _TEXTURE_REQUESTS_H
#define _TEXTURE_REQUESTS_H

#include "TexturePolicies.h"
#include "Frameworks/Request.h"
#include "MathLib/Rect.h"

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
    class RequestCreateTexture : public Frameworks::IRequest
    {
    public:
        RequestCreateTexture(const TexturePolicy& policy) : m_policy(policy) {}
        const TexturePolicy& GetPolicy() { return m_policy; }

    private:
        TexturePolicy m_policy;
    };
    class RequestRetrieveTextureImage : public Frameworks::IRequest
    {
    public:
        RequestRetrieveTextureImage(const std::string& name, const MathLib::Rect& image_rect) : m_name(name), m_imageRect(image_rect) {}
        const std::string& GetTextureName() { return m_name; }
        const MathLib::Rect& GetImageRect() { return m_imageRect; }

    private:
        std::string m_name;
        MathLib::Rect m_imageRect;
    };
}

#endif // _TEXTURE_REQUESTS_H

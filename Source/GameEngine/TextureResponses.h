/*********************************************************************
 * \file   TextureResponses.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _TEXTURE_RESPONSES_H
#define _TEXTURE_RESPONSES_H

#include <system_error>
#include "Frameworks/Response.h"

namespace Enigma::Engine
{
    class Texture;
    class LoadTextureResponse : public Frameworks::IResponse
    {
    public:
        LoadTextureResponse(const Frameworks::Ruid& request_ruid, const std::string& name, const std::shared_ptr<Texture>& tex, std::error_code er)
            : IResponse(request_ruid), m_name(name), m_texture(tex), m_error(er) {};

        const std::string& GetName() { return m_name; }
        std::shared_ptr<Texture> GetTexture() { return m_texture; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::shared_ptr<Texture> m_texture;
        std::error_code m_error;
    };
}

#endif // _TEXTURE_RESPONSES_H

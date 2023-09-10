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
    class CreateTextureResponse : public Frameworks::IResponse
    {
    public:
        CreateTextureResponse(const Frameworks::Ruid& request_ruid, const std::string& name, const std::shared_ptr<Texture>& tex, std::error_code er)
            : IResponse(request_ruid), m_name(name), m_texture(tex), m_error(er) {};

        const std::string& GetName() { return m_name; }
        std::shared_ptr<Texture> GetTexture() { return m_texture; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::shared_ptr<Texture> m_texture;
        std::error_code m_error;
    };
    class RetrieveTextureImageResponse : public Frameworks::IResponse
    {
    public:
        RetrieveTextureImageResponse(const Frameworks::Ruid& request_ruid, const std::string& name, const byte_buffer& image_buff, std::error_code er)
            : IResponse(request_ruid), m_name(name), m_imageBuff(image_buff), m_error(er) {};

        const std::string& GetName() { return m_name; }
        const byte_buffer& GetImageBuffer() { return m_imageBuff; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        byte_buffer m_imageBuff;
        std::error_code m_error;
    };
    class UpdateTextureImageResponse : public Frameworks::IResponse
    {
    public:
        UpdateTextureImageResponse(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponse(request_ruid), m_name(name), m_error(er) {};

        const std::string& GetName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
}

#endif // _TEXTURE_RESPONSES_H

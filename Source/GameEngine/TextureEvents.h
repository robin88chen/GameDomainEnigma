/*********************************************************************
 * \file   TextureEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _TEXTURE_EVENTS_H
#define _TEXTURE_EVENTS_H

#include <system_error>
#include "Frameworks/ExtentTypesDefine.h"
#include "Frameworks/Event.h"

namespace Enigma::Engine
{
    class Texture;
    class TextureLoaded : public Frameworks::IResponseEvent
    {
    public:
        TextureLoaded(const Frameworks::Ruid& request_ruid, const std::string& name, const std::shared_ptr<Texture>& tex)
            : IResponseEvent(request_ruid), m_name(name), m_texture(tex) {};

        const std::string& GetName() { return m_name; }
        std::shared_ptr<Texture> GetTexture() { return m_texture; }
    private:
        std::string m_name;
        std::shared_ptr<Texture> m_texture;
    };
    class LoadTextureFailed : public Frameworks::IResponseEvent
    {
    public:
        LoadTextureFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};

        const std::string& GetName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };

    class TextureCreated : public Frameworks::IResponseEvent
    {
    public:
        TextureCreated(const Frameworks::Ruid& request_ruid, const std::string& name, const std::shared_ptr<Texture>& tex)
            : IResponseEvent(request_ruid), m_name(name), m_texture(tex) {};

        const std::string& GetName() { return m_name; }
        std::shared_ptr<Texture> GetTexture() { return m_texture; }
    private:
        std::string m_name;
        std::shared_ptr<Texture> m_texture;
    };
    class CreateTextureFailed : public Frameworks::IResponseEvent
    {
    public:
        CreateTextureFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};

        const std::string& GetName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };

    class TextureSaved : public Frameworks::IResponseEvent
    {
    public:
        TextureSaved(const Frameworks::Ruid& request_ruid, const std::string& name)
            : IResponseEvent(request_ruid), m_name(name) {};

        const std::string& GetName() { return m_name; }
    private:
        std::string m_name;
    };
    class SaveTextureFailed : public Frameworks::IResponseEvent
    {
    public:
        SaveTextureFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};

        const std::string& GetName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };

    class TextureImageRetrieved : public Frameworks::IResponseEvent
    {
    public:
        TextureImageRetrieved(const Frameworks::Ruid& request_ruid, const std::string& name, const byte_buffer& image_buff)
            : IResponseEvent(request_ruid), m_name(name), m_imageBuff(image_buff) {};

        const std::string& GetName() { return m_name; }
        const byte_buffer& GetImageBuffer() { return m_imageBuff; }
    private:
        std::string m_name;
        byte_buffer m_imageBuff;
    };
    class RetrieveTextureImageFailed : public Frameworks::IResponseEvent
    {
    public:
        RetrieveTextureImageFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};

        const std::string& GetName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };

    class TextureImageUpdated : public Frameworks::IResponseEvent
    {
    public:
        TextureImageUpdated(const Frameworks::Ruid& request_ruid, const std::string& name)
            : IResponseEvent(request_ruid), m_name(name) {};

        const std::string& GetName() { return m_name; }
    private:
        std::string m_name;
    };
    class UpdateTextureImageFailed : public Frameworks::IResponseEvent
    {
    public:
        UpdateTextureImageFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};

        const std::string& GetName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
}

#endif // _TEXTURE_EVENTS_H

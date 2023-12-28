/*********************************************************************
 * \file   TextureEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _TEXTURE_EVENTS_H
#define _TEXTURE_EVENTS_H

#include "Frameworks/Event.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "TextureId.h"
#include <system_error>

namespace Enigma::Engine
{
    class Texture;

    class TextureContented : public Frameworks::IEvent
    {
    public:
        TextureContented(const TextureId& id, const std::shared_ptr<Texture>& tex) : m_id(id), m_texture(tex) {}

        const TextureId& id() { return m_id; }
        std::shared_ptr<Texture> texture() { return m_texture; }
    private:
        TextureId m_id;
        std::shared_ptr<Texture> m_texture;
    };
    class ContentTextureFailed : public Frameworks::IEvent
    {
    public:
        ContentTextureFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {}

        const TextureId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        TextureId m_id;
        std::error_code m_error;
    };

    class TextureSaved : public Frameworks::IResponseEvent
    {
    public:
        TextureSaved(const Frameworks::Ruid& request_ruid, const std::string& name)
            : IResponseEvent(request_ruid), m_name(name) {};

        const std::string& getName() { return m_name; }
    private:
        std::string m_name;
    };
    class SaveTextureFailed : public Frameworks::IResponseEvent
    {
    public:
        SaveTextureFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};

        const std::string& getName() { return m_name; }
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

        const std::string& getName() { return m_name; }
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

        const std::string& getName() { return m_name; }
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

        const std::string& getName() { return m_name; }
    private:
        std::string m_name;
    };
    class UpdateTextureImageFailed : public Frameworks::IResponseEvent
    {
    public:
        UpdateTextureImageFailed(const Frameworks::Ruid& request_ruid, const std::string& name, std::error_code er)
            : IResponseEvent(request_ruid), m_name(name), m_error(er) {};

        const std::string& getName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    //---------------- Repository Events ----------------
    class RemoveTextureFailed : public Frameworks::IEvent
    {
    public:
        RemoveTextureFailed(const TextureId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const TextureId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        TextureId m_id;
        std::error_code m_error;
    };
    class PutTextureFailed : public Frameworks::IEvent
    {
    public:
        PutTextureFailed(const TextureId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const TextureId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        TextureId m_id;
        std::error_code m_error;
    };
}

#endif // _TEXTURE_EVENTS_H

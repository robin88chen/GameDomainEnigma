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

    class TextureConstituted : public Frameworks::IEvent
    {
    public:
        TextureConstituted(const TextureId& id, const std::shared_ptr<Texture>& tex, bool is_persisted) : m_id(id), m_texture(tex) {}

        const TextureId& id() { return m_id; }
        std::shared_ptr<Texture> texture() { return m_texture; }
        bool isPersisted() { return m_isPersisted; }
    private:
        TextureId m_id;
        std::shared_ptr<Texture> m_texture;
        bool m_isPersisted;
    };
    class ConstituteTextureFailed : public Frameworks::IEvent
    {
    public:
        ConstituteTextureFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {}

        const TextureId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        TextureId m_id;
        std::error_code m_error;
    };
    class TextureHydrated : public Frameworks::IEvent
    {
    public:
        TextureHydrated(const TextureId& id, const std::shared_ptr<Texture>& tex) : m_id(id), m_texture(tex) {}

        const TextureId& id() { return m_id; }
        std::shared_ptr<Texture> texture() { return m_texture; }
    private:
        TextureId m_id;
        std::shared_ptr<Texture> m_texture;
    };
    class HydrateTextureFailed : public Frameworks::IEvent
    {
    public:
        HydrateTextureFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {}

        const TextureId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        TextureId m_id;
        std::error_code m_error;
    };

    class TextureSaved : public Frameworks::IEvent
    {
    public:
        TextureSaved(const TextureId& id) : m_id(id) {}

        const TextureId& id() { return m_id; }
    private:
        TextureId m_id;
    };
    class SaveTextureFailed : public Frameworks::IEvent
    {
    public:
        SaveTextureFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {}

        const TextureId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        TextureId m_id;
        std::error_code m_error;
    };

    class TextureImageRetrieved : public Frameworks::IEvent
    {
    public:
        TextureImageRetrieved(const TextureId& id, const byte_buffer& image_buff) : m_id(id), m_imageBuff(image_buff) {};

        const TextureId& id() { return m_id; }
        const byte_buffer& imageBuffer() { return m_imageBuff; }
    private:
        TextureId m_id;
        byte_buffer m_imageBuff;
    };
    class RetrieveTextureImageFailed : public Frameworks::IEvent
    {
    public:
        RetrieveTextureImageFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {};

        const TextureId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        TextureId m_id;
        std::error_code m_error;
    };

    class TextureImageUpdated : public Frameworks::IEvent
    {
    public:
        TextureImageUpdated(const TextureId& id) : m_id(id) {};

        const TextureId& id() { return m_id; }
    private:
        TextureId m_id;
    };
    class UpdateTextureImageFailed : public Frameworks::IEvent
    {
    public:
        UpdateTextureImageFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {};

        const TextureId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        TextureId m_id;
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

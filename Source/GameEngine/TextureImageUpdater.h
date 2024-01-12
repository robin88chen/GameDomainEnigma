/*********************************************************************
 * \file   TextureImageUpdater.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2023
 *********************************************************************/
#ifndef TEXTURE_IMAGE_UPDATER_H
#define TEXTURE_IMAGE_UPDATER_H

#include "Frameworks/EventSubscriber.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "MathLib/Rect.h"
#include "TextureId.h"
#include <string>
#include <system_error>

namespace Enigma::Engine
{
    class Texture;
    class TextureRepository;
    class TextureImageUpdater
    {
    public:
        class TextureImageRetrieved : public Frameworks::IEvent
        {
        public:
            TextureImageRetrieved(const TextureId& id, const byte_buffer& buff) : m_id(id), m_buffer(buff) {};
            const TextureId& id() const { return m_id; }
            const byte_buffer& buffer() const { return m_buffer; }

        private:
            TextureId m_id;
            byte_buffer m_buffer;
        };
        class RetrieveTextureFailed : public Frameworks::IEvent
        {
        public:
            RetrieveTextureFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {};
            const TextureId& id() const { return m_id; }
            std::error_code error() const { return m_error; }

        private:
            TextureId m_id;
            std::error_code m_error;
        };
        class TextureImageUpdated : public Frameworks::IEvent
        {
        public:
            TextureImageUpdated(const TextureId& id) : m_id(id) {};
            const TextureId& id() const { return m_id; }

        private:
            TextureId m_id;
        };
        class UpdateTextureFailed : public Frameworks::IEvent
        {
        public:
            UpdateTextureFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {};
            const TextureId& id() const { return m_id; }
            std::error_code error() const { return m_error; }

        private:
            TextureId m_id;
            std::error_code m_error;
        };
    public:
        TextureImageUpdater();
        TextureImageUpdater(const TextureImageUpdater&) = delete;
        TextureImageUpdater(TextureImageUpdater&&) = delete;
        ~TextureImageUpdater();
        TextureImageUpdater& operator=(const TextureImageUpdater&) = delete;
        TextureImageUpdater& operator=(TextureImageUpdater&&) = delete;

        void retrieveTextureImage(const std::shared_ptr<Texture>& target_texture, const MathLib::Rect& image_rect);
        void updateTextureImage(const std::shared_ptr<Texture>& target_texture, const MathLib::Rect& image_rect, const byte_buffer& image_buff);

    private:
        void onResourceImageRetrieved(const Enigma::Frameworks::IEventPtr& e);
        void onRetrieveResourceImageFailed(const Enigma::Frameworks::IEventPtr& e);
        void onResourceImageUpdated(const Enigma::Frameworks::IEventPtr& e);
        void onUpdateResourceImageFailed(const Enigma::Frameworks::IEventPtr& e);

    private:
        std::shared_ptr<Texture> m_targetTexture;
        MathLib::Rect m_targetTextureRect;

        Enigma::Frameworks::EventSubscriberPtr m_onResourceImageRetrieved;
        Enigma::Frameworks::EventSubscriberPtr m_onRetrieveResourceImageFailed;
        Enigma::Frameworks::EventSubscriberPtr m_onResourceImageUpdated;
        Enigma::Frameworks::EventSubscriberPtr m_onUpdateResourceImageFailed;
    };
}

#endif // TEXTURE_IMAGE_UPDATER_H

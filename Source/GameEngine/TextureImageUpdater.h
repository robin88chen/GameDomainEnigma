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
#include <string>
#include <system_error>

#include "RenderBuffer.h"
#include "MathLib/Rect.h"

namespace Enigma::Engine
{
    class TextureRepository;
    class TextureImageUpdater
    {
    public:
        class TextureImageRetrieved : public Frameworks::IEvent
        {
        public:
            TextureImageRetrieved(const std::string& name, const byte_buffer& buff) : m_name(name), m_buffer(buff) {};
            const std::string& GetTextureName() const { return m_name; }
            const byte_buffer& GetImageBuffer() const { return m_buffer; }

        private:
            std::string m_name;
            byte_buffer m_buffer;
        };
        class RetrieveTextureFailed : public Frameworks::IEvent
        {
        public:
            RetrieveTextureFailed(const std::string& name, std::error_code er) : m_name(name), m_error(er) {};
            const std::string& GetTextureName() const { return m_name; }
            std::error_code GetError() const { return m_error; }

        private:
            std::string m_name;
            std::error_code m_error;
        };
        class TextureImageUpdated : public Frameworks::IEvent
        {
        public:
            TextureImageUpdated(const std::string& name) : m_name(name) {};
            const std::string& GetTextureName() const { return m_name; }

        private:
            std::string m_name;
        };
        class UpdateTextureFailed : public Frameworks::IEvent
        {
        public:
            UpdateTextureFailed(const std::string& name, std::error_code er) : m_name(name), m_error(er) {};
            const std::string& GetTextureName() const { return m_name; }
            std::error_code GetError() const { return m_error; }

        private:
            std::string m_name;
            std::error_code m_error;
        };
    public:
        TextureImageUpdater(TextureRepository* host);
        TextureImageUpdater(const TextureImageUpdater&) = delete;
        TextureImageUpdater(TextureImageUpdater&&) = delete;
        ~TextureImageUpdater();
        TextureImageUpdater& operator=(const TextureImageUpdater&) = delete;
        TextureImageUpdater& operator=(TextureImageUpdater&&) = delete;

        void RetrieveTextureImage(const std::string& name, const MathLib::Rect& image_rect);
        void UpdateTextureImage(const std::string& name, const MathLib::Rect& image_rect, const byte_buffer& image_buff);

    private:
        void OnResourceImageRetrieved(const Enigma::Frameworks::IEventPtr& e);
        void OnResourceImageUpdated(const Enigma::Frameworks::IEventPtr& e);

    private:
        TextureRepository* m_hostRepository;

        std::string m_targetTextureName;
        MathLib::Rect m_targetTextureRect;

        Enigma::Frameworks::EventSubscriberPtr m_onResourceImageRetrieved;
        Enigma::Frameworks::EventSubscriberPtr m_onResourceImageUpdated;
    };
}

#endif // TEXTURE_IMAGE_UPDATER_H

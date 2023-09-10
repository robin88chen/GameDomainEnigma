/*********************************************************************
 * \file   TextureImageRetriever.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2023
 *********************************************************************/
#ifndef TEXTURE_IMAGE_RETRIEVER_H
#define TEXTURE_IMAGE_RETRIEVER_H

#include "Frameworks/EventSubscriber.h"
#include <string>
#include <system_error>

#include "RenderBuffer.h"
#include "MathLib/Rect.h"

namespace Enigma::Engine
{
    class TextureRepository;
    class TextureImageRetriever
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
    public:
        TextureImageRetriever(TextureRepository* host);
        TextureImageRetriever(const TextureImageRetriever&) = delete;
        TextureImageRetriever(TextureImageRetriever&&) = delete;
        ~TextureImageRetriever();
        TextureImageRetriever& operator=(const TextureImageRetriever&) = delete;
        TextureImageRetriever& operator=(TextureImageRetriever&&) = delete;

        void RetrieveTextureImage(const std::string& name, const MathLib::Rect& image_rect);

    private:
        void OnResourceImageRetrieved(const Enigma::Frameworks::IEventPtr& e);

    private:
        TextureRepository* m_hostRepository;

        std::string m_retrievingTextureName;
        MathLib::Rect m_retrievingTextureRect;

        Enigma::Frameworks::EventSubscriberPtr m_onResourceImageRetrieved;
    };
}

#endif // TEXTURE_IMAGE_RETRIEVER_H

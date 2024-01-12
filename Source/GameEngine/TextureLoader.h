/*********************************************************************
 * \file   TextureLoader.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include "Frameworks/EventSubscriber.h"
#include "Texture.h"
#include "TextureDto.h"
#include <memory>

namespace Enigma::Engine
{
    class TextureRepository;
    class TextureLoader
    {
    public:
        class TextureLoaded : public Frameworks::IEvent
        {
        public:
            TextureLoaded(const TextureId& id, const std::shared_ptr<Texture>& tex) :
                m_id(id), m_texture(tex) {};
            const TextureId& id() const { return m_id; }
            const std::shared_ptr<Texture>& texture() const { return m_texture; }
        private:
            TextureId m_id;
            std::shared_ptr<Texture> m_texture;
        };
        class LoadTextureFailed : public Frameworks::IEvent
        {
        public:
            LoadTextureFailed(const TextureId& id, std::error_code er) :
                m_id(id), m_error(er) {};
            const TextureId& id() const { return m_id; }
            std::error_code error() const { return m_error; }
        private:
            TextureId m_id;
            std::error_code m_error;
        };
    public:
        TextureLoader();
        TextureLoader(const TextureLoader&) = delete;
        TextureLoader(TextureLoader&&) = delete;
        ~TextureLoader();
        TextureLoader& operator=(TextureLoader&) = delete;
        TextureLoader& operator=(TextureLoader&&) = delete;

        void contentImage(const std::shared_ptr<Texture>& texture, const TextureDto& dto);

    private:
        void onDeviceTextureCreated(const Enigma::Frameworks::IEventPtr& e);
        void onTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e);
        void onTextureLoadImageFailed(const Enigma::Frameworks::IEventPtr& e);
        void onTextureResourceCreated(const Enigma::Frameworks::IEventPtr& e);
        void onTextureCreateResourceFailed(const Enigma::Frameworks::IEventPtr& e);

        void loadResourceTextures(const std::shared_ptr<Graphics::ITexture>& dev_tex);
        void createEmptyResourceTextures(const std::shared_ptr<Graphics::ITexture>& dev_tex);

        void failContentingImage(std::error_code er);
    private:
        TextureDto m_textureDto;
        std::shared_ptr<Texture> m_contentingTexture;

        Enigma::Frameworks::EventSubscriberPtr m_onTextureCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onMultiTextureCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureImageLoaded;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureLoadImageFailed;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureResourceCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onMultiTextureResourceCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureCreateResourceFailed;
    };
}

#endif // TEXTURE_LOADER_H

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
#include "TexturePolicies.h"
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
        class TextureSaved : public Frameworks::IEvent
        {
        public:
            TextureSaved(const std::string& name) : m_name(name) {};
            const std::string& GetTextureName() const { return m_name; }
        private:
            std::string m_name;
        };
        class SaveTextureFailed : public Frameworks::IEvent
        {
        public:
            SaveTextureFailed(const std::string& name, std::error_code er) :
                m_name(name), m_error(er) {};
            const std::string& GetTextureName() const { return m_name; }
            std::error_code GetError() const { return m_error; }
        private:
            std::string m_name;
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
        void SaveTexture(const std::shared_ptr<Texture>& target_texture, const std::string& name, const FileSystem::IFilePtr& file);

    private:
        void onDeviceTextureCreated(const Enigma::Frameworks::IEventPtr& e);
        void onTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e);
        void onTextureLoadImageFailed(const Enigma::Frameworks::IEventPtr& e);
        void onTextureResourceCreated(const Enigma::Frameworks::IEventPtr& e);
        void onTextureCreateResourceFailed(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureImageSaved(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureSaveImageFailed(const Enigma::Frameworks::IEventPtr& e);

        void loadResourceTextures(const std::shared_ptr<Graphics::ITexture>& dev_tex);
        void createEmptyResourceTextures(const std::shared_ptr<Graphics::ITexture>& dev_tex);

        void failContentingImage(std::error_code er);
    private:
        TextureDto m_textureDto;
        std::shared_ptr<Texture> m_contentingTexture;

        TextureRepository* m_hostRepository;
        TexturePolicy m_policy;
        std::string m_savingDeviceTextureName;
        std::string m_savingTextureName;

        Enigma::Frameworks::EventSubscriberPtr m_onTextureCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onMultiTextureCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureImageLoaded;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureLoadImageFailed;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureResourceCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onMultiTextureResourceCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureCreateResourceFailed;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureImageSaved;
        Enigma::Frameworks::EventSubscriberPtr m_onTextureSaveImageFailed;
    };
}

#endif // TEXTURE_LOADER_H

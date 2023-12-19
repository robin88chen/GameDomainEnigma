/*********************************************************************
 * \file   TextureLoader.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H

#include <memory>

#include "TexturePolicies.h"
#include "Frameworks/EventSubscriber.h"
#include "Texture.h"

namespace Enigma::Engine
{
    class TextureRepository;
    class TextureLoader
    {
    public:
        class TextureLoaded : public Frameworks::IEvent
        {
        public:
            TextureLoaded(const std::string& name, const TexturePtr& tex) :
                m_name(name), m_texture(tex) {};
            const std::string& GetTextureName() const { return m_name; }
            const TexturePtr& getTexture() const { return m_texture; }
        private:
            std::string m_name;
            TexturePtr m_texture;
        };
        class LoadTextureFailed : public Frameworks::IEvent
        {
        public:
            LoadTextureFailed(const std::string& name, std::error_code er) :
                m_name(name), m_error(er) {};
            const std::string& GetTextureName() const { return m_name; }
            std::error_code GetError() const { return m_error; }
        private:
            std::string m_name;
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
        TextureLoader(TextureRepository* host);
        TextureLoader(const TextureLoader&) = delete;
        TextureLoader(TextureLoader&&) = delete;
        ~TextureLoader();
        TextureLoader& operator=(TextureLoader&) = delete;
        TextureLoader& operator=(TextureLoader&&) = delete;

        void LoadTexture(const TexturePolicy& policy);
        void SaveTexture(const std::shared_ptr<Texture>& target_texture, const std::string& name, const FileSystem::IFilePtr& file);

    private:
        void OnTextureCreated(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureImageLoaded(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureLoadImageFailed(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureResourceCreated(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureCreateResourceFailed(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureImageSaved(const Enigma::Frameworks::IEventPtr& e);
        void OnTextureSaveImageFailed(const Enigma::Frameworks::IEventPtr& e);

    private:
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

/*********************************************************************
 * \file   TextureSaver.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef TEXTURE_SAVER_H
#define TEXTURE_SAVER_H

#include "TextureId.h"
#include "Frameworks/Event.h"
#include "Frameworks/EventSubscriber.h"
#include "FileSystem/IFile.h"
#include <system_error>

namespace Enigma::Engine
{
    class Texture;
    class TextureSaver
    {
    public:
        class TextureSaved : public Frameworks::IEvent
        {
        public:
            TextureSaved(const TextureId& id) : m_id(id) {}
            const TextureId& id() const { return m_id; }
        private:
            TextureId m_id;
        };
        class SaveTextureFailed : public Frameworks::IEvent
        {
        public:
            SaveTextureFailed(const TextureId& id, std::error_code er) : m_id(id), m_error(er) {}
            const TextureId& id() const { return m_id; }
            std::error_code error() const { return m_error; }
        private:
            TextureId m_id;
            std::error_code m_error;
        };
    public:
        TextureSaver();
        TextureSaver(const TextureSaver&) = delete;
        TextureSaver(TextureSaver&&) = delete;
        ~TextureSaver();
        TextureSaver& operator=(const TextureSaver&) = delete;
        TextureSaver& operator=(TextureSaver&&) = delete;

        void saveTextureImage(const std::shared_ptr<Texture>& target_texture, const FileSystem::IFilePtr& file);

    private:
        void onTextureImageSaved(const Frameworks::IEventPtr& e);
        void onSaveTextureImageFailed(const Frameworks::IEventPtr& e);

        void failSavingImage(const TextureId& id, std::error_code er);

    private:
        Frameworks::EventSubscriberPtr m_onTextureImageSaved;
        Frameworks::EventSubscriberPtr m_onSaveTextureImageFailed;

        std::shared_ptr<Texture> m_savingTexture;
    };
}

#endif // TEXTURE_SAVER_H

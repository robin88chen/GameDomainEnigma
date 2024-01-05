/*********************************************************************
 * \file   TextureResourceProcessor.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef TEXTURE_RESOURCE_PROCESSOR_H
#define TEXTURE_RESOURCE_PROCESSOR_H

#include "GenericDto.h"
#include "TextureDto.h"
#include "Frameworks/EventSubscriber.h"
#include "FileSystem/IFile.h"
#include <system_error>
#include <memory>
#include <mutex>
#include <queue>

namespace Enigma::Engine
{
    class Texture;
    class TextureLoader;
    class TextureSaver;

    class TextureResourceProcessor
    {
    public:
        TextureResourceProcessor();
        ~TextureResourceProcessor();

        std::error_code enqueueContentingDto(const std::shared_ptr<Texture>& texture, const GenericDto& dto);
        std::error_code contentNextTextureResource();

        std::error_code enqueueSavingTexture(const std::shared_ptr<Texture>& texture, const std::shared_ptr<FileSystem::IFile>& file);
        std::error_code saveNextTextureResource();

    private:
        void registerHandlers();
        void unregisterHandlers();

        void onLoaderTextureLoaded(const Frameworks::IEventPtr& e);
        void onLoaderLoadTextureFailed(const Frameworks::IEventPtr& e);
        void onSaverTextureSaved(const Frameworks::IEventPtr& e);
        void onSaverSaveTextureFailed(const Frameworks::IEventPtr& e);

    private:
        TextureLoader* m_loader;
        TextureSaver* m_saver;
        std::queue<std::pair<std::shared_ptr<Texture>, TextureDto>> m_contentingQueue;
        std::recursive_mutex m_contentingQueueLock;
        std::shared_ptr<Texture> m_currentContentingTexture;

        std::queue<std::pair<std::shared_ptr<Texture>, std::shared_ptr<FileSystem::IFile>>> m_savingQueue;
        std::recursive_mutex m_savingQueueLock;
        std::shared_ptr<Texture> m_currentSavingTexture;

        Frameworks::EventSubscriberPtr m_onLoaderTextureLoaded;
        Frameworks::EventSubscriberPtr m_onLoaderLoadTextureFailed;
        Frameworks::EventSubscriberPtr m_onSaverTextureSaved;
        Frameworks::EventSubscriberPtr m_onSaverSaveTextureFailed;
    };
}

#endif // TEXTURE_RESOURCE_PROCESSOR_H

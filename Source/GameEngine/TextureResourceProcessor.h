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
#include "Frameworks/CommandSubscriber.h"
#include "FileSystem/IFile.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "MathLib/Rect.h"
#include <system_error>
#include <memory>
#include <mutex>
#include <queue>

namespace Enigma::Engine
{
    class Texture;
    class TextureLoader;
    class TextureSaver;
    class TextureImageUpdater;

    class TextureResourceProcessor
    {
    public:
        TextureResourceProcessor();
        ~TextureResourceProcessor();

        std::error_code enqueueContentingDto(const std::shared_ptr<Texture>& texture, const GenericDto& dto);
        std::error_code contentNextTextureResource();

        std::error_code enqueueSavingTexture(const std::shared_ptr<Texture>& texture, const std::shared_ptr<FileSystem::IFile>& file);
        std::error_code saveNextTextureResource();

        std::error_code enqueueRetrievingTexture(const std::shared_ptr<Texture>& texture, const MathLib::Rect& rect);
        std::error_code enqueueUpdatingTexture(const std::shared_ptr<Texture>& texture, const MathLib::Rect& rect, const byte_buffer& buff);
        std::error_code processNextTextureResource();

    private:
        void registerHandlers();
        void unregisterHandlers();

        void onLoaderTextureLoaded(const Frameworks::IEventPtr& e);
        void onLoaderLoadTextureFailed(const Frameworks::IEventPtr& e);
        void onSaverTextureSaved(const Frameworks::IEventPtr& e);
        void onSaverSaveTextureFailed(const Frameworks::IEventPtr& e);
        void onUpdaterImageRetrieved(const Frameworks::IEventPtr& e);
        void onUpdaterRetrieveImageFailed(const Frameworks::IEventPtr& e);
        void onUpdaterImageUpdated(const Frameworks::IEventPtr& e);
        void onUpdaterUpdateImageFailed(const Frameworks::IEventPtr& e);

        void enqueueSavingTexture(const Frameworks::ICommandPtr& c);
        void enqueueRetrievingImage(const Frameworks::ICommandPtr& c);
        void enqueueUpdatingImage(const Frameworks::ICommandPtr& c);
    private:
        TextureLoader* m_loader;
        TextureSaver* m_saver;
        TextureImageUpdater* m_imageUpdater;
        std::queue<std::pair<std::shared_ptr<Texture>, TextureDto>> m_contentingQueue;
        std::recursive_mutex m_contentingQueueLock;
        std::shared_ptr<Texture> m_currentContentingTexture;

        std::queue<std::pair<std::shared_ptr<Texture>, std::shared_ptr<FileSystem::IFile>>> m_savingQueue;
        std::recursive_mutex m_savingQueueLock;
        std::shared_ptr<Texture> m_currentSavingTexture;

        std::queue<std::pair<std::shared_ptr<Texture>, MathLib::Rect>> m_retrievingQueue;
        std::recursive_mutex m_retrievingQueueLock;
        std::queue<std::tuple<std::shared_ptr<Texture>, MathLib::Rect, byte_buffer>> m_updatingQueue;
        std::recursive_mutex m_updatingQueueLock;
        std::shared_ptr<Texture> m_currentProcessingTexture;

        Frameworks::EventSubscriberPtr m_onLoaderTextureLoaded;
        Frameworks::EventSubscriberPtr m_onLoaderLoadTextureFailed;
        Frameworks::EventSubscriberPtr m_onSaverTextureSaved;
        Frameworks::EventSubscriberPtr m_onSaverSaveTextureFailed;
        Frameworks::EventSubscriberPtr m_onUpdaterImageRetrieved;
        Frameworks::EventSubscriberPtr m_onUpdaterRetrieveImageFailed;
        Frameworks::EventSubscriberPtr m_onUpdaterImageUpdated;
        Frameworks::EventSubscriberPtr m_onUpdaterUpdateImageFailed;

        Frameworks::CommandSubscriberPtr m_enqueueSavingTexture;
        Frameworks::CommandSubscriberPtr m_enqueueRetrievingImage;
        Frameworks::CommandSubscriberPtr m_enqueueUpdatingImage;
    };
}

#endif // TEXTURE_RESOURCE_PROCESSOR_H

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
#include <system_error>
#include <memory>
#include <mutex>
#include <queue>

namespace Enigma::Engine
{
    class Texture;
    class TextureLoader;

    class TextureResourceProcessor
    {
    public:
        TextureResourceProcessor();
        ~TextureResourceProcessor();

        std::error_code enqueueContentingDto(const std::shared_ptr<Texture>& texture, const GenericDto& dto);
        std::error_code contentNextTextureResource();

    private:
        void registerHandlers();
        void unregisterHandlers();

        void onLoaderTextureLoaded(const Frameworks::IEventPtr& e);
        void onLoaderLoadTextureFailed(const Frameworks::IEventPtr& e);

    private:
        TextureLoader* m_loader;
        std::queue<std::pair<std::shared_ptr<Texture>, TextureDto>> m_contentingQueue;
        std::recursive_mutex m_contentingQueueLock;
        std::shared_ptr<Texture> m_currentContentingTexture;

        Frameworks::EventSubscriberPtr m_onLoaderTextureLoaded;
        Frameworks::EventSubscriberPtr m_onLoaderLoadTextureFailed;
    };
}

#endif // TEXTURE_RESOURCE_PROCESSOR_H

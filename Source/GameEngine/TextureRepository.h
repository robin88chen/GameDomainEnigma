/*********************************************************************
 * \file   TextureRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2022
 *********************************************************************/
#ifndef TEXTURE_REPOSITORY_H
#define TEXTURE_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "Texture.h"
#include "Frameworks/Event.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/QuerySubscriber.h"
#include "TextureCommands.h"
#include "TexturePolicies.h"
#include "TextureId.h"
#include <queue>

namespace Enigma::Engine
{
    using error = std::error_code;

    class TextureLoader;
    class TextureImageUpdater;
    class TextureStoreMapper;
    class TextureFactory;

    class TextureRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        TextureRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<TextureStoreMapper>& store_mapper);
        TextureRepository(const TextureRepository&) = delete;
        TextureRepository(TextureRepository&&) = delete;
        ~TextureRepository() override;
        TextureRepository& operator=(const TextureRepository&) = delete;
        TextureRepository& operator=(TextureRepository&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

        TextureFactory* factory() { return m_factory; }
        //error LoadTexture(const TexturePolicy& policy);

        bool hasTexture(const TextureId& id);
        std::shared_ptr<Texture> queryTexture(const TextureId& id);
        void removeTexture(const TextureId& id);
        void putTexture(const TextureId& id, const std::shared_ptr<Texture>& texture);

    private:
        //void OnTextureLoaded(const Frameworks::IEventPtr& e);
        //void OnLoadTextureFailed(const Frameworks::IEventPtr& e);
        //void OnTextureSaved(const Frameworks::IEventPtr& e);
        //void OnSaveTextureFailed(const Frameworks::IEventPtr& e);
        void OnTextureImageRetrieved(const Frameworks::IEventPtr& e);
        void OnRetrieveTextureImageFailed(const Frameworks::IEventPtr& e);
        void OnTextureImageUpdated(const Frameworks::IEventPtr& e);
        void OnUpdateTextureImageFailed(const Frameworks::IEventPtr& e);

        void removeTexture(const Frameworks::ICommandPtr& c);
        void putTexture(const Frameworks::ICommandPtr& c);

        //void DoLoadingTexture(const Frameworks::ICommandPtr& c);
        //void DoSavingTexture(const Frameworks::ICommandPtr& c);
        void DoRetrievingTextureImage(const Frameworks::ICommandPtr& c);
        void DoUpdatingTextureImage(const Frameworks::ICommandPtr& c);

        void queryTexture(const Frameworks::IQueryPtr& q);
        void InvokeRequest(const std::shared_ptr<Frameworks::IRequestCommand>& request);

    private:
        std::shared_ptr<TextureStoreMapper> m_storeMapper;
        TextureFactory* m_factory;

        //Frameworks::EventSubscriberPtr m_onTextureLoaded;
        //Frameworks::EventSubscriberPtr m_onLoadTextureFailed;
        //Frameworks::EventSubscriberPtr m_onTextureSaved;
        //Frameworks::EventSubscriberPtr m_onSaveTextureFailed;
        Frameworks::EventSubscriberPtr m_onTextureImageRetrieved;
        Frameworks::EventSubscriberPtr m_onRetrieveTextureImageFailed;
        Frameworks::EventSubscriberPtr m_onTextureImageUpdated;
        Frameworks::EventSubscriberPtr m_onUpdateTextureImageFailed;

        Frameworks::CommandSubscriberPtr m_removeTexture;
        Frameworks::CommandSubscriberPtr m_putTexture;

        //Frameworks::CommandSubscriberPtr m_doLoadingTexture;
        //Frameworks::CommandSubscriberPtr m_doCreatingTexture;
        //Frameworks::CommandSubscriberPtr m_doSavingTexture;
        Frameworks::CommandSubscriberPtr m_doRetrievingTextureImage;
        Frameworks::CommandSubscriberPtr m_doUpdatingTextureImage;

        Frameworks::QuerySubscriberPtr m_queryTexture;

        using TextureMap = std::unordered_map<TextureId, std::shared_ptr<Texture>, TextureId::hash>;
        TextureMap m_textures;
        std::recursive_mutex m_textureMapLock;

        TextureLoader* m_loader;
        TextureImageUpdater* m_updater;
        std::queue<std::shared_ptr<Frameworks::IRequestCommand>> m_requests;
        Frameworks::Ruid m_currentRequestRuid;
        TexturePolicy::JobType m_currentJob;
        bool m_currentRequesting;
        std::mutex m_requestsLock;
    };
}

#endif // TEXTURE_REPOSITORY_H

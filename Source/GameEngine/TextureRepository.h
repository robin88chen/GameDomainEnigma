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
#include "Frameworks/RequestSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "TextureRequests.h"
#include "TexturePolicies.h"
#include <queue>

namespace Enigma::Engine
{
    using error = std::error_code;

    class TextureLoader;

    class TextureRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        TextureRepository(Frameworks::ServiceManager* srv_manager);
        TextureRepository(const TextureRepository&) = delete;
        TextureRepository(TextureRepository&&) = delete;
        ~TextureRepository() override;
        TextureRepository& operator=(const TextureRepository&) = delete;
        TextureRepository& operator=(TextureRepository&&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        //error LoadTexture(const TexturePolicy& policy);

        bool HasTexture(const std::string& name);
        std::shared_ptr<Texture> QueryTexture(const std::string& name);

    private:
        void OnTextureLoaded(const Frameworks::IEventPtr& e);
        void OnLoadTextureFailed(const Frameworks::IEventPtr& e);
        void DoLoadingTexture(const Frameworks::IRequestPtr& r);

    private:
        Frameworks::EventSubscriberPtr m_onTextureLoaded;
        Frameworks::EventSubscriberPtr m_onLoadTextureFailed;
        Frameworks::RequestSubscriberPtr m_doLoadingTexture;
        Frameworks::RequestSubscriberPtr m_doCreatingTexture;

        using TextureMap = std::unordered_map<std::string, std::weak_ptr<Texture>>;

        TextureMap m_textures;
        std::recursive_mutex m_textureMapLock;

        TextureLoader* m_loader;
        std::queue<std::shared_ptr<RequestLoadTexture>> m_loadRequests;
        std::queue<std::shared_ptr<RequestCreateTexture>> m_createRequests;
        Frameworks::Ruid m_currentRequestRuid;
        TexturePolicy::JobType m_currentJob;
        std::mutex m_requestsLock;
    };
}

#endif // TEXTURE_REPOSITORY_H

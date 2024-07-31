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
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/QuerySubscriber.h"
#include "TextureId.h"
#include <unordered_map>

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
        virtual Frameworks::ServiceResult onTerm() override;

        bool hasTexture(const TextureId& id);
        std::shared_ptr<Texture> queryTexture(const TextureId& id);
        void removeTexture(const TextureId& id);
        void putTexture(const TextureId& id, const std::shared_ptr<Texture>& texture);

    private:
        void registerHandlers();
        void unregisterHandlers();

        void removeTexture(const Frameworks::ICommandPtr& c);
        void putTexture(const Frameworks::ICommandPtr& c);
        void queryTexture(const Frameworks::IQueryPtr& q);
        void requestTextureConstitution(const Frameworks::IQueryPtr& q);

        void dumpRetainedTexture();

    private:
        std::shared_ptr<TextureStoreMapper> m_storeMapper;
        TextureFactory* m_factory;

        Frameworks::CommandSubscriberPtr m_removeTexture;
        Frameworks::CommandSubscriberPtr m_putTexture;

        Frameworks::QuerySubscriberPtr m_queryTexture;
        Frameworks::QuerySubscriberPtr m_requestTextureConstitution;

        //! ADR: 在 repository 中，map 是存放已生成 asset 的 cache, 不擁有asset, 所以改用 weak_ptr
        using TextureMap = std::unordered_map<TextureId, std::weak_ptr<Texture>, TextureId::hash>;
        TextureMap m_textures;
        std::recursive_mutex m_textureMapLock;
    };
}

#endif // TEXTURE_REPOSITORY_H

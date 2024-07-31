/*********************************************************************
 * \file   TextureFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 *********************************************************************/
#ifndef EDITOR_TEXTURE_FILE_STORE_MAPPER_H
#define EDITOR_TEXTURE_FILE_STORE_MAPPER_H

#include "FileStorage/TextureFileStoreMapper.h"
#include "Frameworks/QuerySubscriber.h"

namespace LevelEditor
{
    class TextureFileStoreMapper : public Enigma::FileStorage::TextureFileStoreMapper
    {
    public:
        TextureFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway);
        ~TextureFileStoreMapper() override;

        void subscribeHandlers();
        void unsubscribeHandlers();

        std::optional<Enigma::Engine::TextureId> textureId(const std::string& texture_name) const;

    protected:
        void resolveTextureId(const Enigma::Frameworks::IQueryPtr& q);

    protected:
        Enigma::Frameworks::QuerySubscriberPtr m_resolveTextureId;
    };
}

#endif // EDITOR_TEXTURE_FILE_STORE_MAPPER_H

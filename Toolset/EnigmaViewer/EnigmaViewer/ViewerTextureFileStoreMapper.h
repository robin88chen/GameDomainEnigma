/*****************************************************************
 * \file   ViewerTextureFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef VIEWER_TEXTURE_FILE_STORE_MAPPER_H
#define VIEWER_TEXTURE_FILE_STORE_MAPPER_H

#include "FileStorage/TextureFileStoreMapper.h"
#include "Frameworks/QuerySubscriber.h"

namespace EnigmaViewer
{
    class ViewerTextureFileStoreMapper : public Enigma::FileStorage::TextureFileStoreMapper
    {
    public:
        ViewerTextureFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway);
        ~ViewerTextureFileStoreMapper() override;

        void subscribeHandlers();
        void unsubscribeHandlers();

        std::optional<Enigma::Engine::TextureId> textureId(const std::string& texture_name) const;

    protected:
        void resolveTextureId(const Enigma::Frameworks::IQueryPtr& q);

    protected:
        Enigma::Frameworks::QuerySubscriberPtr m_resolveTextureId;
    };
}

#endif // VIEWER_TEXTURE_FILE_STORE_MAPPER_H

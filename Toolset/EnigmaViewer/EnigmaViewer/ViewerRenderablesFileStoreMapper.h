/*****************************************************************
 * \file   ViewerRenderablesFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef VIEWER_RENDERABLES_FILE_STORE_MAPPER_H
#define VIEWER_RENDERABLES_FILE_STORE_MAPPER_H

#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "Frameworks/QuerySubscriber.h"

namespace EnigmaViewer
{
    class ViewerRenderablesFileStoreMapper : public Enigma::FileStorage::PrimitiveFileStoreMapper
    {
    public:
        ViewerRenderablesFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway);
        ~ViewerRenderablesFileStoreMapper() override;

        void subscribeHandlers();
        void unsubscribeHandlers();

        std::vector<std::string> modelNames() const;
        std::optional<Enigma::Primitives::PrimitiveId> modelId(const std::string& model_name) const;

    private:
        void requestModelNames(const Enigma::Frameworks::IQueryPtr& q);
        void resolveModelId(const Enigma::Frameworks::IQueryPtr& q);

    private:
        Enigma::Frameworks::QuerySubscriberPtr m_requestModelNames;
        Enigma::Frameworks::QuerySubscriberPtr m_resolveModelId;
    };
}

#endif // VIEWER_RENDERABLES_FILE_STORE_MAPPER_H

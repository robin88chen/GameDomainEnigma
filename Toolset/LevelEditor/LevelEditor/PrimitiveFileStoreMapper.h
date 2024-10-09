/*********************************************************************
 * \file   PrimitiveFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2024
 *********************************************************************/
#ifndef EDITOR_PRIMITIVE_FILE_STORE_MAPPER_H
#define EDITOR_PRIMITIVE_FILE_STORE_MAPPER_H

#include "FileStorage/PrimitiveFileStoreMapper.h"
#include "Frameworks/QuerySubscriber.h"

namespace LevelEditor
{
    class PrimitiveFileStoreMapper : public Enigma::FileStorage::PrimitiveFileStoreMapper
    {
    public:
        PrimitiveFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway);
        PrimitiveFileStoreMapper(const PrimitiveFileStoreMapper&) = delete;
        PrimitiveFileStoreMapper& operator=(const PrimitiveFileStoreMapper&) = delete;
        virtual ~PrimitiveFileStoreMapper() override;
        PrimitiveFileStoreMapper(PrimitiveFileStoreMapper&&) = delete;
        PrimitiveFileStoreMapper& operator=(PrimitiveFileStoreMapper&&) = delete;

        void subscribeHandlers();
        void unsubscribeHandlers();

    protected:
        void queryPrimitiveDto(const Enigma::Frameworks::IQueryPtr& q);

    protected:
        Enigma::Frameworks::QuerySubscriberPtr m_queryPrimitiveDto;
    };
}

#endif // EDITOR_PRIMITIVE_FILE_STORE_MAPPER_H

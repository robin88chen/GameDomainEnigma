#include "PrimitiveFileStoreMapper.h"

#include <Frameworks/CommandSubscriber.h>

#include "LevelEditorQueries.h"

using namespace LevelEditor;

PrimitiveFileStoreMapper::PrimitiveFileStoreMapper(const std::string& mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway)
    : Enigma::FileStorage::PrimitiveFileStoreMapper(mapper_filename, gateway)
{
}

PrimitiveFileStoreMapper::~PrimitiveFileStoreMapper()
{
}

void PrimitiveFileStoreMapper::subscribeHandlers()
{
    m_queryPrimitiveDto = Enigma::Frameworks::requestSubscription(typeid(QueryPrimitiveDto), [=](const auto q) { queryPrimitiveDto(q); });
}

void PrimitiveFileStoreMapper::unsubscribeHandlers()
{
    Enigma::Frameworks::releaseSubscription(typeid(QueryPrimitiveDto), m_queryPrimitiveDto);
    m_queryPrimitiveDto = nullptr;
}

void PrimitiveFileStoreMapper::queryPrimitiveDto(const Enigma::Frameworks::IQueryPtr& q)
{
    if (!q) return;
    std::shared_ptr<QueryPrimitiveDto> query = std::dynamic_pointer_cast<QueryPrimitiveDto>(q);
    if (!query) return;
    query->setResult(queryPrimitive(query->id()));
}

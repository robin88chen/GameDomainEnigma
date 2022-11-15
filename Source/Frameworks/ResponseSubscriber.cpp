#include "ResponseSubscriber.h"

using namespace Enigma::Frameworks;

ResponseSubscriber::ResponseSubscriber()
{
}

ResponseSubscriber::ResponseSubscriber(const ResponseHandler& handler) : m_handler(handler)
{
}

ResponseSubscriber::~ResponseSubscriber()
{
}

void ResponseSubscriber::HandleResponse(const IResponsePtr& r)
{
    if (m_handler) m_handler(r);
}


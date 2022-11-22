#include "RequestSubscriber.h"

using namespace Enigma::Frameworks;

RequestSubscriber::RequestSubscriber()
{
}

RequestSubscriber::RequestSubscriber(const RequestHandler& handler) : m_handler(handler)
{
}

RequestSubscriber::~RequestSubscriber()
{
}

void RequestSubscriber::HandleRequest(const IRequestPtr& r)
{
    if (m_handler) m_handler(r);
}


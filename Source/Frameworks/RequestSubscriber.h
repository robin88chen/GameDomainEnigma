/*********************************************************************
 * \file   RequestSubscriber.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _REQUEST_SUBSCRIBER_H
#define _REQUEST_SUBSCRIBER_H

#include "Request.h"

namespace Enigma::Frameworks
{
    class RequestSubscriber
    {
    public:
        RequestSubscriber();
        RequestSubscriber(const RequestHandler& handler);
        virtual ~RequestSubscriber();

        virtual void HandleRequest(const IRequestPtr& r);

    protected:
        RequestHandler m_handler;
    };
    using RequestSubscriberPtr = std::shared_ptr<RequestSubscriber>;
}

#endif // _REQUEST_SUBSCRIBER_H

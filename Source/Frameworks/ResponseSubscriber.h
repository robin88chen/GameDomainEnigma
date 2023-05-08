/*********************************************************************
 * \file   ResponseSubscriber.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _RESPONSE_SUBSCRIBER_H
#define _RESPONSE_SUBSCRIBER_H

#include "Response.h"

namespace Enigma::Frameworks
{
    class ResponseSubscriber
    {
    public:
        ResponseSubscriber();
        ResponseSubscriber(const ResponseHandler& handler);
        ResponseSubscriber(const ResponseSubscriber&) = delete;
        ResponseSubscriber(ResponseSubscriber&&) = delete;
        virtual ~ResponseSubscriber();
        ResponseSubscriber& operator=(const ResponseSubscriber&) = delete;
        ResponseSubscriber& operator=(ResponseSubscriber&&) = delete;

        virtual void HandleResponse(const IResponsePtr& r);

    protected:
        ResponseHandler m_handler;
    };
    using ResponseSubscriberPtr = std::shared_ptr<ResponseSubscriber>;
}


#endif // _RESPONSE_SUBSCRIBER_H

/*********************************************************************
 * \file   Request.h
 * \brief  Request interface
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef REQUEST_H
#define REQUEST_H

#include "ruid.h"
#include <functional>
#include <memory>

namespace Enigma::Frameworks
{
    class IRequest
    {
    public:
        IRequest();
        IRequest(const Ruid& ruid) : m_ruid(ruid) {}
        IRequest(const IRequest&) = delete;
        IRequest(IRequest&&) = delete;
        virtual ~IRequest() = default;
        IRequest& operator=(const IRequest&) = delete;
        IRequest& operator=(IRequest&&) = delete;
        virtual const std::type_info& TypeInfo() { return typeid(*this); };  ///< 實作層的 type info

        const Ruid& GetRuid() const { return m_ruid; }

    protected:
        Ruid m_ruid;
    };
    using IRequestPtr = std::shared_ptr<IRequest>;
    using RequestHandler = std::function<void(const IRequestPtr&)>;
    using RequestHandlerDelegate = void(*)(const IRequestPtr&);
}

#endif // REQUEST_H

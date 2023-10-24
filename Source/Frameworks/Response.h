/*********************************************************************
 * \file   Response.h
 * \brief  Response interface
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef RESPONSE_H
#define RESPONSE_H

#include "ruid.h"
#include <functional>
#include <memory>
namespace Enigma::Frameworks
{
    class IResponse
    {
    public:
        IResponse(const Ruid& request_ruid);
        IResponse(const IResponse&) = delete;
        IResponse(IResponse&&) = delete;
        virtual ~IResponse() = default;
        IResponse& operator=(const IResponse&) = delete;
        IResponse& operator=(IResponse&&) = delete;
        virtual const std::type_info& typeInfo() { return typeid(*this); };  ///< 實作層的 type info

        const Ruid& getRequestRuid() const { return m_ruid; }

    protected:
        Ruid m_ruid;
    };
    using IResponsePtr = std::shared_ptr<IResponse>;
    using ResponseHandler = std::function<void(const IResponsePtr&)>;
    using ResponseHandlerDelegate = void(*)(const IResponsePtr&);
}

#endif // RESPONSE_H

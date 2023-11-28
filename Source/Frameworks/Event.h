/*********************************************************************
 * \file   Event.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef EVENT_H
#define EVENT_H

#include "ruid.h"
#include <functional>
#include <memory>

namespace Enigma::Frameworks
{
    class IEvent
    {
    public:
        IEvent() {};
        IEvent(const IEvent&) = delete;
        IEvent(IEvent&&) = delete;
        virtual ~IEvent() {};
        IEvent& operator=(const IEvent&) = delete;
        IEvent& operator=(IEvent&&) = delete;
        virtual const std::type_info& typeInfo() { return typeid(*this); };  ///< 實作層的 type info
    };

    class IResponseEvent : public IEvent
    {
    public:
        IResponseEvent(const Ruid& request_ruid) : m_ruid(request_ruid) {};
        IResponseEvent(const IResponseEvent&) = delete;
        IResponseEvent(IResponseEvent&&) = delete;
        virtual ~IResponseEvent() {};
        IResponseEvent& operator=(const IResponseEvent&) = delete;
        IResponseEvent& operator=(IResponseEvent&&) = delete;

        const Ruid& getRequestRuid() const { return m_ruid; }

    protected:
        Ruid m_ruid;
    };
    using IEventPtr = std::shared_ptr<IEvent>;
    using EventHandler = std::function<void(const IEventPtr&)>;
    using EventHandlerDelegate = void(*)(const IEventPtr&);
}

#endif // EVENT_H

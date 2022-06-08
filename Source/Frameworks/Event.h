/*********************************************************************
 * \file   Event.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef EVENT_H
#define EVENT_H
#include <functional>
#include <memory>

namespace Enigma::Frameworks
{
    class IEvent
    {
    public:
        IEvent() {};
        IEvent(const IEvent&) = delete;
        virtual ~IEvent() {};
        IEvent& operator=(const IEvent&) = delete;
        virtual const std::type_info& TypeInfo() { return typeid(*this); };  ///< 實作層的 type info
    };
    using IEventPtr = std::shared_ptr<IEvent>;
    using EventHandler = std::function<void(const IEventPtr&)>;
    using EventHandlerDelegate = void(*)(const IEventPtr&);
}

#endif // EVENT_H

/*********************************************************************
 * \file   SystemService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef _SYSTEM_SERVICE_H
#define _SYSTEM_SERVICE_H

#include "Rtti.h"

namespace Enigma::Frameworks
{
    class ServiceManager;

    enum class ServiceResult
    {
        Pendding,
        Failure,
        Complete,
    };

    /** system service interface */
    class ISystemService
    {
    public:
        static Rtti TYPE_RTTI;
        virtual const Rtti& TypeInfo() const { return TYPE_RTTI; }
    public:
        ISystemService(ServiceManager*);
        ISystemService(const ISystemService&) = delete;
        virtual ~ISystemService();
        ISystemService& operator=(const ISystemService&) = delete;
        ISystemService& operator=(ISystemService&&) = delete;

        inline ServiceManager* GetServiceManager() { return m_serviceManager; };
        inline const ServiceManager* GetServiceManager() const { return m_serviceManager; };

        /// On PreInit
        virtual ServiceResult OnPreInit();
        /// On Init
        virtual ServiceResult OnInit();
        /// On Term
        virtual ServiceResult OnTerm();

        /// On Tick (if need tick)
        virtual ServiceResult OnTick();
        inline bool IsNeedTick() { return m_needTick; };

        /// suspend
        inline void Suspend() { m_isSuspended = true; };
        /// resume
        inline void Resume() { m_isSuspended = false; };
        /// is suspended?
        inline bool IsSuspended() { return m_isSuspended; };

        /// order value, for sorting in service manager, small value will called early
        unsigned int GetOrderValue() { return m_orderValue; };
        enum
        {
            TimerServiceOrderValue = 0,
            MessageServiceOrderValue = 10,
            ServiceDefaultOrderValue = 1000,
        };

    protected:
        ServiceManager* m_serviceManager;
        bool m_needTick;
        bool m_isSuspended;
        unsigned int m_orderValue;
    };
};

#endif // !_SYSTEM_SERVICE_H

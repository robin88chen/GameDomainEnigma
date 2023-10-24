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
        DECLARE_EN_RTTI_OF_BASE;
    public:
        ISystemService(ServiceManager*);
        ISystemService(const ISystemService&) = delete;
        ISystemService(ISystemService&&) = delete;
        virtual ~ISystemService();
        ISystemService& operator=(const ISystemService&) = delete;
        ISystemService& operator=(ISystemService&&) = delete;

        inline ServiceManager* GetServiceManager() { return m_serviceManager; };
        inline const ServiceManager* GetServiceManager() const { return m_serviceManager; };

        /// On PreInit
        virtual ServiceResult onPreInit();
        /// On Init
        virtual ServiceResult onInit();
        /// On Term
        virtual ServiceResult onTerm();

        /// On Tick (if need tick)
        virtual ServiceResult onTick();
        inline bool isNeedTick() { return m_needTick; };

        /// suspend
        inline void suspend() { m_isSuspended = true; };
        /// resume
        inline void resume() { m_isSuspended = false; };
        /// is suspended?
        inline bool isSuspended() { return m_isSuspended; };

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

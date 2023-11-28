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

        inline ServiceManager* getServiceManager() { return m_serviceManager; };
        inline const ServiceManager* getServiceManager() const { return m_serviceManager; };

        /// On PreInit
        virtual ServiceResult onPreInit();
        /// On Init
        virtual ServiceResult onInit();
        /// On Term
        virtual ServiceResult onTerm();

        /// On Tick (if need tick)
        virtual ServiceResult onTick();
        inline bool isNeedTick() const { return m_needTick; };

        /// suspend
        inline void suspend() { m_isSuspended = true; };
        /// resume
        inline void resume() { m_isSuspended = false; };
        /// is suspended?
        inline bool isSuspended() const { return m_isSuspended; };


    protected:
        ServiceManager* m_serviceManager;
        bool m_needTick;
        bool m_isSuspended;
    };
};

#endif // !_SYSTEM_SERVICE_H

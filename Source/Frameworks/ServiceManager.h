/*********************************************************************
 * \file   ServiceManager.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef _SERVICE_MANAGER_H
#define _SERVICE_MANAGER_H

#include "SystemService.h"
#include "Rtti.h"
#include <list>
#include <optional>
#include <unordered_map>
#include <memory>

namespace Enigma::Frameworks
{
    /** service manager */
    class ServiceManager
    {
    public:
        enum class ServiceState
        {
            Invalid,
            PreInit,
            Initializing,
            WaitingToRun,
            Running,
            WaitingToShutdown,
            ShuttingDown,
            Complete,
            Deleted,
        };

    public:
        ServiceManager();
        ServiceManager(const ServiceManager&) = delete;
        ServiceManager(ServiceManager&&) = delete;
        virtual ~ServiceManager();
        ServiceManager& operator=(const ServiceManager&) = delete;
        ServiceManager& operator=(ServiceManager&&) = delete;

        void registerSystemService(const std::shared_ptr<ISystemService>& service);
        void unregisterSystemService(const Rtti& service_type);
        void insertHashAsService(const Rtti& service_type, const std::shared_ptr<ISystemService>& service);
        void removeHashAsService(const Rtti& service_type);
        /// shutdown immediately
        void shutdownSystemService(const Rtti& service_type);

        void runOnce();
        /// run service once if service is on state
        void runForState(ServiceState st);

        ServiceState checkServiceState(const Rtti& service_type);

        std::shared_ptr<ISystemService> getSystemService(const Rtti& service_type);
        std::optional<std::shared_ptr<ISystemService>> tryGetSystemService(const Rtti& service_type);

        template <class T>
        std::shared_ptr<T> getSystemServiceAs()
        {
            return std::dynamic_pointer_cast<T, ISystemService>(getSystemService(T::TYPE_RTTI));
        };

    protected:
        struct ServiceStateRecord
        {
            ServiceStateRecord() : m_state(ServiceState::Invalid), m_service(nullptr), m_isRegistered(false) {};
            ServiceState m_state;
            std::shared_ptr<ISystemService> m_service;
            bool m_isRegistered;
        };

        typedef std::list<ServiceStateRecord> SystemServiceList;

    protected:
        typedef std::unordered_map<const Rtti*, std::shared_ptr<ISystemService>> SystemServiceMap;  ///< mapping by rtti type_index

        SystemServiceList m_services;
        SystemServiceMap m_mapServices;

        ServiceState m_minServiceState;  ///< minimun service state
    };
};

#endif // !_SERVICE_MANAGER_H

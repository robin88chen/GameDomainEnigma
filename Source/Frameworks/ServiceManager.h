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

        void RegisterSystemService(const std::shared_ptr<ISystemService>& service);
        void UnregisterSystemService(const Rtti& service_type);
        void InsertHashAsService(const Rtti& service_type, const std::shared_ptr<ISystemService>& service);
        void RemoveHashAsService(const Rtti& service_type);
        /// shutdown immediately
        void ShutdownSystemService(const Rtti& service_type);

        void RunOnce();
        /// run service once if service is on state
        void RunForState(ServiceState st);

        ServiceState CheckServiceState(const Rtti& service_type);

        std::shared_ptr<ISystemService> GetSystemService(const Rtti& service_type);
        std::optional<std::shared_ptr<ISystemService>> TryGetSystemService(const Rtti& service_type);

        template <class T>
        std::shared_ptr<T> GetSystemServiceAs()
        {
            return std::dynamic_pointer_cast<T, ISystemService>(GetSystemService(T::TYPE_RTTI));
        };

    protected:
        struct ServiceStateRecord
        {
            ServiceStateRecord() : m_state(ServiceState::Invalid), m_service(nullptr), m_isRegistered(false) {};
            ServiceState m_state;
            std::shared_ptr<ISystemService> m_service;
            bool m_isRegistered;
        };
        static bool comp_service_order(const ServiceStateRecord& rec1, const ServiceStateRecord& rec2)
        {
            if (rec1.m_service->GetOrderValue() < rec2.m_service->GetOrderValue()) return true;
            return false;
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

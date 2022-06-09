#include "ServiceManager.h"
#include "SystemService.h"
#include "SystemServiceEvents.h"
#include "EventPublisher.h"
#include "MemoryAllocMacro.h"

using namespace Enigma::Frameworks;

ServiceManager* ServiceManager::m_instance = nullptr;

ServiceManager::ServiceManager()
{
    m_instance = this;
    m_minServiceState = ServiceState::Invalid;
}

ServiceManager::~ServiceManager()
{
    m_instance = nullptr;
}

void ServiceManager::RegisterSystemService(ISystemService* service)
{
    if (!service) return;
    ServiceStateRecord rec;
    rec.m_state = ServiceState::PreInit;
    rec.m_service = service;
    rec.m_isRegistered = true;
    m_services.emplace_back(rec);
    m_services.sort(comp_service_order);
    m_mapServices[service->TypeInfo()] = service;
}

void ServiceManager::UnregisterSystemService(const Rtti& service_type)
{
    SystemServiceList::iterator iter = m_services.begin();
    while (iter != m_services.end())
    {
        if (((*iter).m_service) && ((*iter).m_service->TypeInfo() == service_type))
        {
            (*iter).m_isRegistered = false;
            if ((*iter).m_state == ServiceState::Running) (*iter).m_state = ServiceState::WaitingToShutdown;
        }
        ++iter;
    }
}

void ServiceManager::InsertHashAsService(const Rtti& service_type, ISystemService* service)
{
    if (!service) return;
    m_mapServices[service_type] = service;
}

void ServiceManager::RemoveHashAsService(const Rtti& service_type)
{
    m_mapServices.erase(service_type);
}

void ServiceManager::ShutdownSystemService(const Rtti& service_type)
{
    SystemServiceList::iterator iter = m_services.begin();
    while (iter != m_services.end())
    {
        if (((*iter).m_service) && ((*iter).m_service->TypeInfo() == service_type))
        {
            (*iter).m_isRegistered = false;
            if ((*iter).m_state == ServiceState::Running)
            {
                (*iter).m_state = ServiceState::ShuttingDown;
                ServiceResult res = (*iter).m_service->OnTerm();
                if (res == ServiceResult::Complete)
                {
                    m_mapServices[service_type] = nullptr;
                    delete (*iter).m_service;
                    (*iter).m_service = nullptr;
                    (*iter).m_state = ServiceState::Deleted;
                }
            }
        }
        ++iter;
    }
}

void ServiceManager::RunOnce()
{
    if (m_services.empty()) return;

    ServiceState tempMinState = ServiceState::Deleted;

    for (SystemServiceList::iterator iterService = m_services.begin();
        iterService != m_services.end(); ++iterService)
    {
        if (!((*iterService).m_service)) continue;
        if (((*iterService).m_service)->IsSuspended()) continue;

        ServiceResult result = ServiceResult::Complete;
        switch ((*iterService).m_state)
        {
            case ServiceState::Running:
            {
                if ((*iterService).m_service->IsNeedTick())
                {
                    result = (*iterService).m_service->OnTick();
                }
                else
                {
                    result = ServiceResult::Pendding;
                }
            }
            break;
            case ServiceState::PreInit:
            {
                result = (*iterService).m_service->OnPreInit();
            }
            break;
            case ServiceState::Initializing:
            {
                result = (*iterService).m_service->OnInit();
            }
            break;
            case ServiceState::ShuttingDown:
            {
                result = (*iterService).m_service->OnTerm();
            }
            break;
            case ServiceState::Complete:
            {  // 完成，砍掉service
                m_mapServices[(*iterService).m_service->TypeInfo()] = nullptr;
                delete (*iterService).m_service;
                (*iterService).m_service = nullptr;
            }
            break;
            default:
                break;
        }

        if (result == ServiceResult::Complete)
        {
            unsigned int state = (unsigned int)((*iterService).m_state);
            state++;
            (*iterService).m_state = (ServiceState)(state);
        }
        if (tempMinState > (*iterService).m_state) tempMinState = (*iterService).m_state;
    }

    if (tempMinState != m_minServiceState)
    {
        if ((m_minServiceState <= ServiceState::Initializing)
            && (tempMinState > ServiceState::Initializing))
        {
            EventPublisher::Post(IEventPtr{ menew AllServiceInitialized() });
        }
        m_minServiceState = tempMinState;
    }
}

void ServiceManager::RunForState(ServiceState st)
{
    if (m_services.empty()) return;

    ServiceState tempMinState = ServiceState::Deleted;

    for (SystemServiceList::iterator iterService = m_services.begin();
        iterService != m_services.end(); ++iterService)
    {
        if (!((*iterService).m_service)) continue;
        if (((*iterService).m_service)->IsSuspended()) continue;
        if ((*iterService).m_state != st) continue;

        ServiceResult result = ServiceResult::Complete;
        switch (st)
        {
            case ServiceState::Running:
            {
                if ((*iterService).m_service->IsNeedTick())
                {
                    result = (*iterService).m_service->OnTick();
                }
                else
                {
                    result = ServiceResult::Pendding;
                }
            }
            break;
            case ServiceState::PreInit:
            {
                result = (*iterService).m_service->OnPreInit();
            }
            break;
            case ServiceState::Initializing:
            {
                result = (*iterService).m_service->OnInit();
            }
            break;
            case ServiceState::ShuttingDown:
            {
                result = (*iterService).m_service->OnTerm();
            }
            break;
            case ServiceState::Complete:
            {  // 完成，砍掉service
                m_mapServices[(*iterService).m_service->TypeInfo()] = nullptr;
                delete (*iterService).m_service;
                (*iterService).m_service = nullptr;
            }
            break;
            default:
                break;
        }

        if (result == ServiceResult::Complete)
        {
            unsigned int state = (unsigned int)((*iterService).m_state);
            state++;
            (*iterService).m_state = (ServiceState)(state);
        }
        if (tempMinState > (*iterService).m_state) tempMinState = (*iterService).m_state;
    }

    if (tempMinState != m_minServiceState)
    {
        if ((m_minServiceState <= ServiceState::Initializing)
            && (tempMinState > ServiceState::Initializing))
        {
            EventPublisher::Post(IEventPtr{ menew AllServiceInitialized() });
        }
        m_minServiceState = tempMinState;
    }
}

ServiceManager::ServiceState ServiceManager::CheckServiceState(const Rtti& service_type)
{
    auto service = TryGetSystemService(service_type);
    if (!service) return ServiceState::Invalid;
    if (!service.value()) return ServiceState::Deleted;
    for (SystemServiceList::iterator iterService = m_services.begin();
        iterService != m_services.end(); ++iterService)
    {
        if (((*iterService).m_service) && ((*iterService).m_service->TypeInfo().IsDerived(service_type)))
        {
            return (*iterService).m_state;
        }
    }
    return ServiceState::Invalid;
}

ISystemService* ServiceManager::GetSystemService(const Rtti& service_type)
{
    auto service = TryGetSystemService(service_type);
    if (service) return service.value();
    return nullptr;
}

std::optional<ISystemService*> ServiceManager::TryGetSystemService(const Rtti& service_type)
{
    SystemServiceMap::iterator iter = m_instance->m_mapServices.find(service_type);
    if (iter != m_instance->m_mapServices.end())
    {
        return iter->second;
    }

    // map中沒有完全符合的，找service type的繼承者
    for (iter = m_instance->m_mapServices.begin(); iter != m_instance->m_mapServices.end(); ++iter)
    {
        if (iter->first.IsDerived(service_type))
        {
            return iter->second;
        }
    }
    return std::nullopt;
}

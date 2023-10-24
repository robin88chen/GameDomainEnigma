#include "ServiceManager.h"
#include "SystemService.h"
#include "SystemServiceEvents.h"
#include "EventPublisher.h"

using namespace Enigma::Frameworks;

ServiceManager::ServiceManager()
{
    m_minServiceState = ServiceState::Invalid;
}

ServiceManager::~ServiceManager()
{
}

void ServiceManager::RegisterSystemService(const std::shared_ptr<ISystemService>& service)
{
    if (!service) return;
    ServiceStateRecord rec;
    rec.m_state = ServiceState::PreInit;
    rec.m_service = service;
    rec.m_isRegistered = true;
    m_services.emplace_back(rec);
    m_services.sort(comp_service_order);
    m_mapServices[service->TypeIndex()] = service;
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

void ServiceManager::InsertHashAsService(const Rtti& service_type, const std::shared_ptr<ISystemService>& service)
{
    if (!service) return;
    m_mapServices[&service_type] = service;
}

void ServiceManager::RemoveHashAsService(const Rtti& service_type)
{
    m_mapServices.erase(&service_type);
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
                ServiceResult res = (*iter).m_service->onTerm();
                if (res == ServiceResult::Complete)
                {
                    m_mapServices[&service_type] = nullptr;
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
        if (((*iterService).m_service)->isSuspended()) continue;

        ServiceResult result = ServiceResult::Complete;
        switch ((*iterService).m_state)
        {
        case ServiceState::Running:
        {
            if ((*iterService).m_service->isNeedTick())
            {
                result = (*iterService).m_service->onTick();
            }
            else
            {
                result = ServiceResult::Pendding;
            }
        }
        break;
        case ServiceState::PreInit:
        {
            result = (*iterService).m_service->onPreInit();
        }
        break;
        case ServiceState::Initializing:
        {
            result = (*iterService).m_service->onInit();
        }
        break;
        case ServiceState::ShuttingDown:
        {
            result = (*iterService).m_service->onTerm();
        }
        break;
        case ServiceState::Complete:
        {  // 完成，砍掉service
            m_mapServices[(*iterService).m_service->TypeIndex()] = nullptr;
            (*iterService).m_service = nullptr;
        }
        break;
        default:
            break;
        }

        if (result == ServiceResult::Complete)
        {
            unsigned int state = static_cast<unsigned int>((*iterService).m_state);
            state++;
            (*iterService).m_state = static_cast<ServiceState>(state);
        }
        if (tempMinState > (*iterService).m_state) tempMinState = (*iterService).m_state;
    }

    if (tempMinState != m_minServiceState)
    {
        if ((m_minServiceState <= ServiceState::Initializing)
            && (tempMinState > ServiceState::Initializing))
        {
            EventPublisher::Post(std::make_shared<AllServiceInitialized>());
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
        if (((*iterService).m_service)->isSuspended()) continue;
        if ((*iterService).m_state != st) continue;

        ServiceResult result = ServiceResult::Complete;
        switch (st)
        {
        case ServiceState::Running:
        {
            if ((*iterService).m_service->isNeedTick())
            {
                result = (*iterService).m_service->onTick();
            }
            else
            {
                result = ServiceResult::Pendding;
            }
        }
        break;
        case ServiceState::PreInit:
        {
            result = (*iterService).m_service->onPreInit();
        }
        break;
        case ServiceState::Initializing:
        {
            result = (*iterService).m_service->onInit();
        }
        break;
        case ServiceState::ShuttingDown:
        {
            result = (*iterService).m_service->onTerm();
        }
        break;
        case ServiceState::Complete:
        {  // 完成，砍掉service
            m_mapServices[(*iterService).m_service->TypeIndex()] = nullptr;
            (*iterService).m_service = nullptr;
        }
        break;
        default:
            break;
        }

        if (result == ServiceResult::Complete)
        {
            unsigned int state = static_cast<unsigned int>((*iterService).m_state);
            state++;
            (*iterService).m_state = static_cast<ServiceState>(state);
        }
        if (tempMinState > (*iterService).m_state) tempMinState = (*iterService).m_state;
    }

    if (tempMinState != m_minServiceState)
    {
        if ((m_minServiceState <= ServiceState::Initializing)
            && (tempMinState > ServiceState::Initializing))
        {
            EventPublisher::Post(std::make_shared<AllServiceInitialized>());
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

std::shared_ptr<ISystemService> ServiceManager::GetSystemService(const Rtti& service_type)
{
    auto service = TryGetSystemService(service_type);
    if (service) return service.value();
    return nullptr;
}

std::optional<std::shared_ptr<ISystemService>> ServiceManager::TryGetSystemService(const Rtti& service_type)
{
    SystemServiceMap::iterator iter = m_mapServices.find(&service_type);
    if (iter != m_mapServices.end())
    {
        return iter->second;
    }

    // map中沒有完全符合的，找service type的繼承者
    for (iter = m_mapServices.begin(); iter != m_mapServices.end(); ++iter)
    {
        if (iter->first->IsDerived(service_type))
        {
            return iter->second;
        }
    }
    return std::nullopt;
}

#include "SystemService.h"
//#include "ServiceManager.h"

using namespace Enigma::Frameworks;

const Rtti ISystemService::TYPE_RTTI = Rtti("En.ISystemService");

ISystemService::ISystemService(ServiceManager* manager)
{
    m_serviceManager = manager;
    m_needTick = true;
    m_isSuspended = false;
    m_orderValue = ServiceDefaultOrderValue;
}

ISystemService::~ISystemService()
{
    m_serviceManager = nullptr;
}

ServiceResult ISystemService::OnPreInit()
{
    return ServiceResult::Complete;
}

ServiceResult ISystemService::OnInit()
{
    return ServiceResult::Complete;
}

ServiceResult ISystemService::OnTerm()
{
    return ServiceResult::Complete;
}

ServiceResult ISystemService::OnTick()
{
    return ServiceResult::Pendding;
}


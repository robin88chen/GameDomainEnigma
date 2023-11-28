#include "SystemService.h"
//#include "ServiceManager.h"

using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(Frameworks, ISystemService);

ISystemService::ISystemService(ServiceManager* manager)
{
    m_serviceManager = manager;
    m_needTick = true;
    m_isSuspended = false;
}

ISystemService::~ISystemService()
{
    m_serviceManager = nullptr;
}

ServiceResult ISystemService::onPreInit()
{
    return ServiceResult::Complete;
}

ServiceResult ISystemService::onInit()
{
    return ServiceResult::Complete;
}

ServiceResult ISystemService::onTerm()
{
    return ServiceResult::Complete;
}

ServiceResult ISystemService::onTick()
{
    return ServiceResult::Pendding;
}


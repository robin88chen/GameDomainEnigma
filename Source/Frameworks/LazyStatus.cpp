#include "LazyStatus.h"

using namespace Enigma::Frameworks;

LazyStatus::LazyStatus()
{
    m_status = Status::Ghost;
}

void LazyStatus::changeStatus(Status s)
{
    m_status = s;
}

bool LazyStatus::isGhost() const
{
    return m_status == Status::Ghost;
}

bool LazyStatus::isReady() const
{
    return m_status == Status::Ready;
}

bool LazyStatus::isInQueue() const
{
    return m_status == Status::InQueue;
}

bool LazyStatus::isLoading() const
{
    return m_status == Status::Loading;
}

bool LazyStatus::isFailed() const
{
    return m_status == Status::Failed;
}

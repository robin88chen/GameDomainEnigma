#include "LazyStatus.h"

using namespace Enigma::Frameworks;

LazyStatus::LazyStatus()
{
    m_status = Status::Ghost;
}

void LazyStatus::ChangeStatus(Status s)
{
    m_status = s;
}

bool LazyStatus::IsGhost() const
{
    return m_status == Status::Ghost;
}

bool LazyStatus::IsReady() const
{
    return m_status == Status::Ready;
}

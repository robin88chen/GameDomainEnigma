#include "Request.h"

using namespace Enigma::Frameworks;

IRequest::IRequest()
    : m_ruid(Ruid::Generate())
{
}

#include "Response.h"

using namespace Enigma::Frameworks;

IResponse::IResponse(const Ruid& request_ruid) : m_ruid(request_ruid)
{
}

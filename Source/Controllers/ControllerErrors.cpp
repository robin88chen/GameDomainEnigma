#include "ControllerErrors.h"

using namespace Enigma::Controllers;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::unknownInstallingPolicy: return "Unknown Installing Policy";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Controller Error Category";
}
namespace Enigma::Controllers
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

#include "ApplicationErrors.h"

using namespace Enigma::Application;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Application Error Category";
}
namespace Enigma::Application
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

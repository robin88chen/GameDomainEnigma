#include "EngineErrors.h"

using namespace Enigma::Engine;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::flipNotPrimary: return "Flip not primary";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Engine Error Category";
}
namespace Enigma::Engine
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

#include "RenderingErrors.h"

using namespace Enigma::Rendering;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::nullHostLight: return "Null host light";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Rendering Error Category";
}
namespace Enigma::Rendering
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

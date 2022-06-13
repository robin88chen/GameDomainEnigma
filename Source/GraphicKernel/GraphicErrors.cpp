#include "GraphicErrors.h"

using namespace Enigma::Graphics;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::ResizeBackSurfaceFail: return "Resize back surface fail";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Graphics Error Category";
}
namespace Enigma::Graphics
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

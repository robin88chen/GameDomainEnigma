#include "GameCommonErrors.h"

using namespace Enigma::GameCommon;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::nullPortalManagement: return "Null portal management";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "GameCommon Error Category";
}
namespace Enigma::GameCommon
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

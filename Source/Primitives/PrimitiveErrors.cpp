#include "PrimitiveErrors.h"

using namespace Enigma::Primitives;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::primitiveFactoryNotExists: return "Primitive factory not exists";
    case ErrorCode::primitiveEntityAlreadyExists: return "Primitive entity already exists";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Primitive Error Category";
}
namespace Enigma::Engine
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

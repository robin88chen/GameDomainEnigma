#include "GeometryErrors.h"

using namespace Enigma::Geometries;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::geometryFactoryNotExists: return "Geometry factory not exists";
    case ErrorCode::geometryEntityAlreadyExists: return "Geometry entity already exists";
    case ErrorCode::invalidArrayIndex: return "Invalid array index";
    case ErrorCode::nullMemoryBuffer: return "Null memory buffer";
    case ErrorCode::zeroVertexSize: return "Zero vertex size";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Geometries Error Category";
}
namespace Enigma::Geometries
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

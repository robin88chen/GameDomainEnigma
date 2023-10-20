#include "PrefabErrors.h"

using namespace Enigma::Prefabs;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::emptyPrefabPath: return "Empty prefab path";
    case ErrorCode::nullDeserializer: return "Null deserializer";
    case ErrorCode::emptyPrefabs: return "Empty prefabs";
    case ErrorCode::invalidPrefab: return "Invalid prefab";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Prefabs Error Category";
}
namespace Enigma::Prefabs
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

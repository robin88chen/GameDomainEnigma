#include "GameCommonErrors.h"

using namespace Enigma::GameCommon;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::nullPortalManagement: return "Null portal management";
    case ErrorCode::nullHostLight: return "Null host light";
    case ErrorCode::nullSceneRoot: return "Null scene root";
    case ErrorCode::nodeNotFound: return "Node not found";
    case ErrorCode::spatialNotFound: return "Spatial not found";
    case ErrorCode::nullFrustum: return "Null frustum";
    case ErrorCode::invalidSceneRootId: return "Invalid scene root id";
    case ErrorCode::sceneRootAlreadyExist: return "Scene root already exist";
    case ErrorCode::invalidPortalManagementId: return "Invalid portal management id";
    case ErrorCode::portalManagementAlreadyExist: return "Portal management already exist";
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

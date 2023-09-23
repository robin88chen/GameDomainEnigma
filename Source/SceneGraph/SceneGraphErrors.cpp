#include "SceneGraphErrors.h"

using namespace Enigma::SceneGraph;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::cameraNotReady: return "Camera not ready";
    case ErrorCode::nullSceneGraph: return "Null scene graph";
    case ErrorCode::dataNotReady: return "Data not ready";
    case ErrorCode::parentNode: return "Parent node error";
    case ErrorCode::nullPrimitive: return "Null primitive";
    case ErrorCode::nullCullerCamera: return "Null culler camera";
    case ErrorCode::unsupportPawnPrimitive: return "Unsupport pawn primitive";
    case ErrorCode::invalidChangingCamera: return "Invalid changing camera";
    case ErrorCode::emptyLightQueryResult: return "Empty light query result";
    case ErrorCode::cameraNotFound: return "Camera not found";
    case ErrorCode::frustumNotFound: return "Frustum not found";
    case ErrorCode::sceneRepositoryFailed: return "Scene repository failed";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Scene Graph Error Category";
}
namespace Enigma::SceneGraph
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

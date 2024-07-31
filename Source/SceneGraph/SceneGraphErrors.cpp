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
    case ErrorCode::nodeNotFound: return "Node not found";
    case ErrorCode::spatialNotFound: return "Spatial not found";
    case ErrorCode::runningSpatialNotFound: return "Running spatial not found";
    case ErrorCode::nullPrimitive: return "Null primitive";
    case ErrorCode::nullCullerCamera: return "Null culler camera";
    case ErrorCode::unsupportPawnPrimitive: return "Unsupport pawn primitive";
    case ErrorCode::invalidChangingCamera: return "Invalid changing camera";
    case ErrorCode::emptyLightQueryResult: return "Empty light query result";
    case ErrorCode::cameraNotFound: return "Camera not found";
    case ErrorCode::frustumNotFound: return "Frustum not found";
    case ErrorCode::sceneRepositoryFailed: return "Scene repository failed";
    case ErrorCode::entityAlreadyExists: return "Entity already existed";
    case ErrorCode::sceneFactoryFailed: return "Scene factory failed";
    case ErrorCode::laziedContentNotFound: return "Lazied content not found";
    case ErrorCode::childDtoNotFound: return "Child dto not found";
    case ErrorCode::spatialFactoryNotFound: return "Spatial factory not found";
    case ErrorCode::factoryRttiMismatch: return "Factory rtti mismatch";
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

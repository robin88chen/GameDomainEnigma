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

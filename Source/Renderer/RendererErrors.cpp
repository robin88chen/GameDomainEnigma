#include "RendererErrors.h"

using namespace Enigma::Renderer;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::flipNotPrimary: return "Flip not primary";
    case ErrorCode::nullBackSurface: return "Null back surface";
    case ErrorCode::renderTargetAlreadyExisted: return "Render target already existed";
    case ErrorCode::renderTargetNotExist: return "Render target not exist";
    case ErrorCode::nullRenderBuffer: return "Null render buffer";
    case ErrorCode::nullEffectMaterial: return "Null effect material";
    case ErrorCode::nullRenderElement: return "Null render element";
    case ErrorCode::nullRenderTarget: return "Null render target";
    case ErrorCode::rendererAlreadyExist: return "Renderer already exist";
    case ErrorCode::rendererNotExist: return "Renderer not exist";
    case ErrorCode::rendererFactoryFail: return "Renderer factory fail";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Renderer Error Category";
}
namespace Enigma::Renderer
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

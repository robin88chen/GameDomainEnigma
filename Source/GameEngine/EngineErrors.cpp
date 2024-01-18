#include "EngineErrors.h"

using namespace Enigma::Engine;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::findStashedAssetFail: return "Find stashed asset fail";
    case ErrorCode::policyIncomplete: return "Policy Incomplete";
    case ErrorCode::deserializeFail: return "Deserialize fail";
    case ErrorCode::fileIOError: return "File IO error";
    case ErrorCode::primitiveFactoryNotExists: return "Primitive factory not exists";
    case ErrorCode::primitiveEntityAlreadyExists: return "Primitive entity already exists";
    case ErrorCode::effectAlreadyCompiled: return "Effect already compiled";
    case ErrorCode::animatorFactoryNotExists: return "Animator factory not exists";
    case ErrorCode::animatorEntityAlreadyExists: return "Animator entity already exists";
    case ErrorCode::animationFactoryNotExists: return "Animation factory not exists";
    case ErrorCode::animationAssetAlreadyExists: return "Animation asset already exists";
    case ErrorCode::effectPassEnd: return "Effect pass ended";
    case ErrorCode::compilingEmptyEffectTech: return "Compiling effect has no effect technique";
    case ErrorCode::noSelectedTechnique: return "No selected effect technique";
    case ErrorCode::nullMaterial: return "Null material";
    case ErrorCode::textureSemantic: return "texture semantic error";
    case ErrorCode::targetTextureNotExists: return "Target texture not exists";
    case ErrorCode::textureAlreadyLoaded: return "Texture already loaded";
    case ErrorCode::textureNotReady: return "Texture not ready";
    case ErrorCode::nullAnimator: return "Null animator";
    case ErrorCode::animatorMultiListening: return "Animator multi listening";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Engine Error Category";
}
namespace Enigma::Engine
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

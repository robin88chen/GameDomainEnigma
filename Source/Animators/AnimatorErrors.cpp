#include "AnimatorErrors.h"

using namespace Enigma::Animators;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::animatorFactoryNotExists: return "Animator factory not exists";
    case ErrorCode::animatorEntityAlreadyExists: return "Animator entity already exists";
    case ErrorCode::animationFactoryNotExists: return "Animation factory not exists";
    case ErrorCode::animationAssetAlreadyExists: return "Animation asset already exists";
    case ErrorCode::nullAnimator: return "Null animator";
    case ErrorCode::animatorMultiListening: return "Animator multi listening";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "Animator Error Category";
}
namespace Enigma::Animators
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

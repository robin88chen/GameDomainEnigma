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
    case ErrorCode::effectPassEnd: return "Effect pass ended";
    case ErrorCode::compilingEmptyEffectTech: return "Compiling effect has no effect technique";
    case ErrorCode::noSelectedTechnique: return "No selected effect technique";
    case ErrorCode::nullMaterial: return "Null material";
    case ErrorCode::invalidArrayIndex: return "Invalid array index";
    case ErrorCode::nullMemoryBuffer: return "Null memory buffer";
    case ErrorCode::zeroVertexSize: return "Zero vertex size";
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

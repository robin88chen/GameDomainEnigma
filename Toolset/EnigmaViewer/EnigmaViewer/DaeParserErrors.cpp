#include "DaeParserErrors.h"

using namespace EnigmaViewer;

const ParserErrorCategory ParserErrorCategory::ms_category;

std::string ParserErrorCategory::message(int err) const
{
    switch (static_cast<ParserError>(err))
    {
    case ParserError::ok: return "OK";
    case ParserError::emptyRoot: return "Empty root";
    case ParserError::noAnimationsLib: return "No animations library";
    case ParserError::noAnimationNode: return "No animation node";
    case ParserError::noAnimationNameNode: return "No animation name node";
    case ParserError::noAnimationKeyNode: return "No animation key node";
    case ParserError::noAnimationChannel: return "No animation channel";
    case ParserError::noAnimationSamplerSource: return "No animation sampler source";
    case ParserError::noAnimationSampler: return "No animation sampler";
    case ParserError::noTimeValueSource: return "No time value source";
    case ParserError::noTimeValueArray: return "No time value array";
    case ParserError::noAnimMatrixSource: return "No animation matrix source";
    case ParserError::noAnimMatrixArray: return "No animation matrix array";
    case ParserError::noTimeValues: return "No time values";
    case ParserError::animMatrixCountMismatch: return "Animation matrix count mismatch";
    case ParserError::noAnimationTarget: return "No animation target";
    case ParserError::noAnimationTargetMesh: return "No animation target mesh";
    }
    return "Unknown";
}

const char* ParserErrorCategory::name() const noexcept
{
    return "Dae Parser Error Category";
}
namespace EnigmaViewer
{
    std::error_code make_error_code(ParserError er)
    {
        return std::error_code(static_cast<int>(er), ParserErrorCategory::get());
    }
}

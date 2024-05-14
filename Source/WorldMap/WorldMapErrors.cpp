#include "WorldMapErrors.h"

using namespace Enigma::WorldMap;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::emptyQuadVolumeList: return "Empty Quad Volume List";
    case ErrorCode::parentQuadNodeNotFound: return "Parent Quad Node Not Found";
    case ErrorCode::parentQuadNodeHydrationFailed: return "Parent Quad Node Hydration Failed";
    case ErrorCode::childQuadNodeConstitutionFailed: return "Child Quad Node Constitution Failed";
    case ErrorCode::quadRootAlreadyExists: return "QuadRoot Already Exists";
    case ErrorCode::worldMapAlreadyExists: return "WorldMap Already Exists";
    }
    return "Unknown";
}

const char* ErrorCategory::name() const noexcept
{
    return "World Map Error Category";
}
namespace Enigma::WorldMap
{
    std::error_code make_error_code(ErrorCode ec)
    {
        return std::error_code(static_cast<int>(ec), ErrorCategory::get());
    }
}

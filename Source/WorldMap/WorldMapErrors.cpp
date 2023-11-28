#include "WorldMapErrors.h"

using namespace Enigma::WorldMap;

const ErrorCategory ErrorCategory::ms_category;

std::string ErrorCategory::message(int err) const
{
    switch (static_cast<ErrorCode>(err))
    {
    case ErrorCode::ok: return "OK";
    case ErrorCode::nullSceneGraphRepository: return "Null SceneGraphRepository";
    case ErrorCode::nullPortalManager: return "Null PortalManager";
    case ErrorCode::emptyQuadRoot: return "QuadRoot is empty";
    case ErrorCode::outOfBounds: return "Out of Bounds";
    case ErrorCode::outOfQuadTreeDepth: return "Out of QuadTree Depth";
    case ErrorCode::invalidBoundingBox: return "Invalid Bounding Box";
    case ErrorCode::nullQuadNode: return "Null QuadNode";
    case ErrorCode::emptyQuadGraph: return "Empty QuadGraph";
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

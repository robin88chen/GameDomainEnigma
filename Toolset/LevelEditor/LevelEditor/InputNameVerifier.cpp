#include "InputNameVerifier.h"
#include "LevelEditorQueries.h"

using namespace LevelEditor;

bool InputNameVerifier::isWorldNameDuplicated(const std::string& world_name)
{
    return std::make_shared<IsWorldNameDuplicated>(world_name)->dispatch();
}

bool InputNameVerifier::isSpatialNameDuplicated(const std::string& spatial_name)
{
    return std::make_shared<IsSpatialNameDuplicated>(spatial_name)->dispatch();
}

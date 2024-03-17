#include "LightingPawn.h"
#include "GameCommonErrors.h"
#include "SceneGraph/Light.h"
#include "LightingPawnDto.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(GameCommon, LightingPawn, Pawn);

LightingPawn::LightingPawn(const SpatialId& id) : Pawn(id)
{
}

LightingPawn::LightingPawn(const SpatialId& id, const Engine::GenericDto& o) : Pawn(id, o)
{
    LightingPawnDto dto = LightingPawnDto::fromGenericDto(o);
    m_hostLight = std::dynamic_pointer_cast<Light>(std::make_shared<QuerySpatial>(dto.hostLightId())->dispatch());
}

LightingPawn::~LightingPawn()
{
}

void LightingPawn::setHostLight(const std::shared_ptr<SceneGraph::Light>& light)
{
    m_hostLight = light;
}

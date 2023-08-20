#include "LightingPawn.h"
#include "GameCommonErrors.h"
#include "SceneGraph/Light.h"

using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(GameCommon, LightingPawn, Pawn);

LightingPawn::LightingPawn(const std::string& name) : Pawn(name)
{
}

LightingPawn::LightingPawn(const Engine::GenericDto& o) : Pawn(o)
{
}

LightingPawn::~LightingPawn()
{
}

void LightingPawn::SetHostLight(const std::shared_ptr<SceneGraph::Light>& light)
{
    m_hostLight = light;
}

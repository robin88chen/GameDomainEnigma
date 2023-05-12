#include "LightVolumePawn.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(GameCommon, LightVolumePawn, Pawn);

LightVolumePawn::LightVolumePawn(const std::string& name) : Pawn(name)
{
    
}

LightVolumePawn::LightVolumePawn(const Engine::GenericDto& o) : Pawn(o)
{
    
}

LightVolumePawn::~LightVolumePawn()
{
    
}

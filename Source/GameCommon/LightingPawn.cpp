#include "LightingPawn.h"
#include "GameCommonErrors.h"
#include "SceneGraph/Light.h"
#include "LightingPawnDto.h"

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

void LightingPawn::resolveFactoryLinkage(const Engine::GenericDto& dto, Engine::FactoryLinkageResolver<Spatial>& resolver)
{
    LightingPawnDto pawn_dto = LightingPawnDto::fromGenericDto(dto);
    if (!pawn_dto.HostLightName().empty())
    {
        resolver.TryResolveLinkage(pawn_dto.HostLightName(), [lifetime = weak_from_this()](auto sp)
        {
            if (!lifetime.expired())
                std::dynamic_pointer_cast<LightingPawn, Spatial>(lifetime.lock())->SetHostLight(std::dynamic_pointer_cast<Light>(sp));
        });
    }
}

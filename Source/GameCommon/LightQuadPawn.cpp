#include "LightQuadPawn.h"
#include "GameCommonErrors.h"
#include "SceneGraph/Light.h"
#include "SceneGraph/LightInfo.h"
#include "LightingPawnDto.h"

using namespace Enigma::GameCommon;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;

DEFINE_RTTI(GameCommon, LightQuadPawn, LightingPawn);

LightQuadPawn::LightQuadPawn(const std::string& name) : LightingPawn(name)
{
}

LightQuadPawn::LightQuadPawn(const Engine::GenericDto& o) : LightingPawn(o)
{
}

LightQuadPawn::~LightQuadPawn()
{
}

GenericDto LightQuadPawn::serializeDto()
{
    LightingPawnDto dto(SerializePawnDto());
    dto.HostLightName() = GetHostLightName();
    return dto.toGenericDto();
}

error LightQuadPawn::_updateSpatialRenderState()
{
    if (!isRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (m_hostLight.expired()) return ErrorCode::nullHostLight;

    ColorRGBA colorLight(m_hostLight.lock()->Info().GetLightColor());
    RenderLightingState lighting_state;
    if (m_hostLight.lock()->Info().GetLightType() == LightInfo::LightType::Ambient)
    {
        lighting_state.SetAmbientLightColor(colorLight);
    }
    else if (m_hostLight.lock()->Info().GetLightType() == LightInfo::LightType::SunLight)
    {
        Vector3 lightDir = m_hostLight.lock()->Info().GetLightDirection();
        lighting_state.SetSunLight(lightDir, colorLight);
    }
    m_spatialRenderState = SpatialRenderState(lighting_state);

    return ErrorCode::ok;
}


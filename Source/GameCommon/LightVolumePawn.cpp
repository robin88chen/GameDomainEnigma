#include "LightVolumePawn.h"
#include "Renderer/MeshPrimitive.h"
#include "GameCommonErrors.h"
#include "LightingPawnDto.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::MathLib;

DEFINE_RTTI(GameCommon, LightVolumePawn, LightingPawn);

std::string LightVolumePawn::m_tokenDefaultVisualTech = "Default";
std::string LightVolumePawn::m_tokenInsideVisualTech = "Inside";

LightVolumePawn::LightVolumePawn(const SpatialId& id) : LightingPawn(id), m_isCameraInside(false)
{
}

LightVolumePawn::LightVolumePawn(const SpatialId& id, const Engine::GenericDto& o) : LightingPawn(id, o)
{

}

LightVolumePawn::~LightVolumePawn()
{

}

GenericDto LightVolumePawn::serializeDto()
{
    LightingPawnDto dto(SerializePawnDto());
    dto.HostLightName() = GetHostLightName();
    return dto.toGenericDto();
}

void LightVolumePawn::ToggleCameraInside(bool is_inside)
{
    if (is_inside != m_isCameraInside)
    {
        MeshPrimitivePtr mesh = std::dynamic_pointer_cast<MeshPrimitive, Primitive>(GetPrimitive());
        if (mesh)
        {
            if (is_inside)
            {
                mesh->SelectVisualTechnique(m_tokenInsideVisualTech);
            }
            else
            {
                mesh->SelectVisualTechnique(m_tokenDefaultVisualTech);
            }
        }
    }
    m_isCameraInside = is_inside;
}

error LightVolumePawn::_updateSpatialRenderState()
{
    if (!isRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (m_hostLight.expired()) return ErrorCode::nullHostLight;

    Vector4 vecLightPosRange(m_hostLight.lock()->Info().GetLightPosition(), m_hostLight.lock()->Info().GetLightRange());
    ColorRGBA colorLight(m_hostLight.lock()->Info().GetLightColor());
    Vector4 vecLightAtten(m_hostLight.lock()->Info().GetLightAttenuation(), 1.0f);
    RenderLightingState lighting_state;
    lighting_state.SetPointLightArray({ vecLightPosRange }, { colorLight }, { vecLightAtten });
    m_spatialRenderState = SpatialRenderState(lighting_state);

    return ErrorCode::ok;
}

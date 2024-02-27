#include "LightVolumePawn.h"
#include "Renderables/MeshPrimitive.h"
#include "GameCommonErrors.h"
#include "LightingPawnDto.h"

using namespace Enigma::GameCommon;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Renderables;
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

std::shared_ptr<LightVolumePawn> LightVolumePawn::create(const SceneGraph::SpatialId& id)
{
    return std::make_shared<LightVolumePawn>(id);
}

std::shared_ptr<LightVolumePawn> LightVolumePawn::constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<LightVolumePawn>(id, dto);
}

GenericDto LightVolumePawn::serializeDto()
{
    LightingPawnDto dto(SerializePawnDto());
    dto.hostLightName() = getHostLightName();
    return dto.toGenericDto();
}

void LightVolumePawn::ToggleCameraInside(bool is_inside)
{
    if (is_inside != m_isCameraInside)
    {
        std::shared_ptr<MeshPrimitive> mesh = std::dynamic_pointer_cast<MeshPrimitive>(getPrimitive());
        if (mesh)
        {
            if (is_inside)
            {
                mesh->selectVisualTechnique(m_tokenInsideVisualTech);
            }
            else
            {
                mesh->selectVisualTechnique(m_tokenDefaultVisualTech);
            }
        }
    }
    m_isCameraInside = is_inside;
}

error LightVolumePawn::_updateSpatialRenderState()
{
    if (!isRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (m_hostLight.expired()) return ErrorCode::nullHostLight;

    Vector4 vecLightPosRange(m_hostLight.lock()->info().getLightPosition(), m_hostLight.lock()->info().getLightRange());
    ColorRGBA colorLight(m_hostLight.lock()->info().getLightColor());
    Vector4 vecLightAtten(m_hostLight.lock()->info().getLightAttenuation(), 1.0f);
    RenderLightingState lighting_state;
    lighting_state.SetPointLightArray({ vecLightPosRange }, { colorLight }, { vecLightAtten });
    m_spatialRenderState = SpatialRenderState(lighting_state);

    return ErrorCode::ok;
}

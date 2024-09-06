#include "LightingPawnDto.h"
#include "LightingPawn.h"
#include "LightVolumePawn.h"

using namespace Enigma::Rendering;
using namespace Enigma::SceneGraph;

static std::string TOKEN_HOST_LIGHT_ID = "HostLightId";
static std::string TOKEN_PRESENT_CAMERA_ID = "PresentCameraId";

/*LightingPawnDto::LightingPawnDto(const Engine::GenericDto& dto) : PawnDto(dto)
{
    m_factoryDesc = Engine::FactoryDesc(LightingPawn::TYPE_RTTI.getName());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_HOST_LIGHT_ID)) hostLightId(v.value());
}

LightingPawnDto::LightingPawnDto(const SceneGraph::PawnDto& dto) : PawnDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), LightingPawn::TYPE_RTTI.getName()));
}

Enigma::Engine::GenericDto LightingPawnDto::toGenericDto() const
{
    Engine::GenericDto dto = PawnDto::toGenericDto();
    if (!m_hostLightId.tokens().empty()) dto.addOrUpdate(TOKEN_HOST_LIGHT_ID, m_hostLightId.tokens());
    return dto;
}

LightVolumePawnDto::LightVolumePawnDto(const Engine::GenericDto& dto) : LightingPawnDto(dto)
{
    m_factoryDesc = Engine::FactoryDesc(LightVolumePawn::TYPE_RTTI.getName());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PRESENT_CAMERA_ID)) presentCameraId(v.value());
}

LightVolumePawnDto::LightVolumePawnDto(const LightingPawnDto& dto) : LightingPawnDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), LightVolumePawn::TYPE_RTTI.getName()));
}

Enigma::Engine::GenericDto LightVolumePawnDto::toGenericDto() const
{
    Engine::GenericDto dto = LightingPawnDto::toGenericDto();
    if (!m_cameraId.tokens().empty()) dto.addOrUpdate(TOKEN_PRESENT_CAMERA_ID, m_cameraId.tokens());
    return dto;
}*/


#include "LightingPawnDto.h"
#include "LightingPawn.h"

using namespace Enigma::Rendering;
using namespace Enigma::SceneGraph;

static std::string TOKEN_HOST_LIGHT_ID = "HostLightId";

LightingPawnDto::LightingPawnDto(const Engine::GenericDto& dto) : PawnDto(dto)
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



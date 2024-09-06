#include "LightingPawnAssemblers.h"
#include "LightQuadPawn.h"
#include "LightVolumePawn.h"
using namespace Enigma::Rendering;

static std::string TOKEN_HOST_LIGHT_ID = "HostLightId";
static std::string TOKEN_PRESENT_CAMERA_ID = "PresentCameraId";

LightQuadPawnAssembler::LightQuadPawnAssembler(const SceneGraph::SpatialId& id) : PawnAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(LightQuadPawn::TYPE_RTTI);
}

Enigma::Engine::GenericDto LightQuadPawnAssembler::assemble() const
{
    Engine::GenericDto dto = PawnAssembler::assemble();
    if (m_hostLightId) dto.addOrUpdate(TOKEN_HOST_LIGHT_ID, m_hostLightId->tokens());
    return dto;
}

LightQuadPawnDisassembler::LightQuadPawnDisassembler() : PawnDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(LightQuadPawn::TYPE_RTTI);
}

void LightQuadPawnDisassembler::disassemble(const Engine::GenericDto& dto)
{
    PawnDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_HOST_LIGHT_ID)) m_hostLightId = SceneGraph::SpatialId(v.value());
}

LightVolumePawnAssembler::LightVolumePawnAssembler(const SceneGraph::SpatialId& id) : PawnAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(LightVolumePawn::TYPE_RTTI);
}

Enigma::Engine::GenericDto LightVolumePawnAssembler::assemble() const
{
    Engine::GenericDto dto = PawnAssembler::assemble();
    if (m_hostLightId) dto.addOrUpdate(TOKEN_HOST_LIGHT_ID, m_hostLightId->tokens());
    if (m_cameraId) dto.addOrUpdate(TOKEN_PRESENT_CAMERA_ID, m_cameraId->tokens());
    return dto;
}

LightVolumePawnDisassembler::LightVolumePawnDisassembler() : PawnDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(LightVolumePawn::TYPE_RTTI);
}

void LightVolumePawnDisassembler::disassemble(const Engine::GenericDto& dto)
{
    PawnDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_HOST_LIGHT_ID)) m_hostLightId = SceneGraph::SpatialId(v.value());
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PRESENT_CAMERA_ID)) m_cameraId = SceneGraph::SpatialId(v.value());
}
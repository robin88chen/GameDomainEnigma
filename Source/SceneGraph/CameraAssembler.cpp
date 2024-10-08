#include "CameraAssembler.h"
#include "Camera.h"
#include "FrustumAssembler.h"
#include "Frustum.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_ID = "Id";
static std::string TOKEN_HAND_SYSTEM = "HandSystem";
static std::string TOKEN_EYE_POSITION = "EyePosition";
static std::string TOKEN_LOOK_AT_DIR = "LookAtDir";
static std::string TOKEN_UP_VECTOR = "UpVector";
static std::string TOKEN_FRUSTUM = "Frustum";

CameraAssembler::CameraAssembler(const SpatialId& id) : m_factoryDesc(Camera::TYPE_RTTI), m_id(id), m_handSys(GraphicCoordSys::LeftHand)
{
}

void CameraAssembler::frustum(const std::shared_ptr<FrustumAssembler>& frustum)
{
    m_frustumAssembler = frustum;
}

void CameraAssembler::frustum(const Frustum& frustum)
{
    m_frustumAssembler = std::make_shared<FrustumAssembler>(frustum.projectionType());
    frustum.assemble(m_frustumAssembler);
}

void CameraAssembler::asNative(const std::string& file_at_path)
{
    m_factoryDesc.claimAsNative(file_at_path);
}

Enigma::Engine::GenericDto CameraAssembler::assemble()
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.name());
    dto.addOrUpdate(TOKEN_HAND_SYSTEM, static_cast<unsigned>(m_handSys));
    dto.addOrUpdate(TOKEN_EYE_POSITION, m_eyePosition);
    dto.addOrUpdate(TOKEN_LOOK_AT_DIR, m_lookAtDir);
    dto.addOrUpdate(TOKEN_UP_VECTOR, m_up);
    if (m_frustumAssembler) dto.addOrUpdate(TOKEN_FRUSTUM, m_frustumAssembler->assemble());
    return dto;
}

std::shared_ptr<CameraAssembler> CameraAssembler::assembledAssemblerOf(const std::shared_ptr<Camera>& camera)
{
    assert(camera);
    std::shared_ptr<CameraAssembler> assembler = std::make_shared<CameraAssembler>(camera->id());
    camera->assemble(assembler);
    return assembler;
}

Enigma::Engine::GenericDto CameraAssembler::assemble(const std::shared_ptr<Camera>& camera)
{
    assert(camera);
    return assembledAssemblerOf(camera)->assemble();
}

CameraDisassembler::CameraDisassembler() : m_factoryDesc(Camera::TYPE_RTTI)
{
}

CameraDisassembler::CameraDisassembler(const Engine::GenericDto& dto) : m_factoryDesc(dto.getRtti())
{
    disassemble(dto);
}

const Frustum& CameraDisassembler::frustum() const
{
    assert(m_frustum);
    return *m_frustum;
}

void CameraDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_ID)) m_id = SpatialId(v.value(), Camera::TYPE_RTTI);
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_HAND_SYSTEM)) m_handSys = static_cast<GraphicCoordSys>(v.value());
    if (const auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_EYE_POSITION)) m_eyePosition = v.value();
    if (const auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_LOOK_AT_DIR)) m_lookAtDir = v.value();
    if (const auto v = dto.tryGetValue<MathLib::Vector3>(TOKEN_UP_VECTOR)) m_up = v.value();
    if (const auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_FRUSTUM))
    {
        std::shared_ptr<FrustumDisassembler> disassembler = std::make_shared<FrustumDisassembler>();
        disassembler->disassemble(v.value());
        m_frustum = std::make_shared<Frustum>();
        m_frustum->disassemble(disassembler);
    }
}

std::shared_ptr<CameraDisassembler> CameraDisassembler::disassembledDisassemblerOf(const std::shared_ptr<Camera>& camera, const Engine::GenericDto& dto)
{
    assert(camera);
    std::shared_ptr<CameraDisassembler> disassembler = std::make_shared<CameraDisassembler>();
    disassembler->disassemble(dto);
    return disassembler;
}

void CameraDisassembler::disassemble(const std::shared_ptr<Camera>& camera, const Engine::GenericDto& dto)
{
    assert(camera);
    camera->disassemble(disassembledDisassemblerOf(camera, dto));
}

#include "PawnAssembler.h"
#include "Pawn.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_PAWN_PRIMITIVE_ID = "PawnPrimitiveId";

PawnAssembler::PawnAssembler(const SpatialId& id) : SpatialAssembler(id)
{
    m_factoryDesc = Engine::FactoryDesc(Pawn::TYPE_RTTI.getName());
}

void PawnAssembler::primitiveId(const Primitives::PrimitiveId& primitive_id)
{
    m_primitiveId = primitive_id;
}

Enigma::Engine::GenericDto PawnAssembler::assemble() const
{
    Engine::GenericDto dto = SpatialAssembler::assemble();
    if (m_primitiveId)
    {
        dto.addOrUpdate(TOKEN_PAWN_PRIMITIVE_ID, m_primitiveId.value().tokens());
    }
    return dto;
}

PawnDisassembler::PawnDisassembler() : SpatialDisassembler()
{
}

void PawnDisassembler::disassemble(const Engine::GenericDto& dto)
{
    SpatialDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PAWN_PRIMITIVE_ID)) m_primitiveId = v.value();
}
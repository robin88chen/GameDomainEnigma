#include "TerrainPrimitiveFactoryMethods.h"
#include "TerrainPrimitive.h"
#include "TerrainPrimitiveAssembler.h"

namespace Enigma::Terrain
{
    std::shared_ptr<TerrainPrimitive> createTerrainPrimitive(const Enigma::Primitives::PrimitiveId& id)
    {
        return std::make_shared<TerrainPrimitive>(id);
    }

    std::shared_ptr<TerrainPrimitive> constituteTerrainPrimitive(const Enigma::Primitives::PrimitiveId& id, const Enigma::Engine::GenericDto& dto)
    {
        auto primitive = std::make_shared<TerrainPrimitive>(id);
        auto disassembler = primitive->disassembler();
        disassembler->disassemble(dto);
        primitive->disassemble(disassembler);
        return primitive;
    }
}
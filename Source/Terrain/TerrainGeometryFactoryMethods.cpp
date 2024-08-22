#include "TerrainGeometryFactoryMethods.h"
#include "TerrainGeometry.h"
#include "TerrainGeometryAssembler.h"

using namespace Enigma::Terrain;

std::shared_ptr<TerrainGeometry> createTerrainGeometry(const Enigma::Geometries::GeometryId& id)
{
    return std::make_shared<TerrainGeometry>(id);
}

std::shared_ptr<TerrainGeometry> constituteTerrainGeometry(const Enigma::Geometries::GeometryId& id, const Enigma::Engine::GenericDto& dto)
{
    auto geometry = std::make_shared<TerrainGeometry>(id);
    auto disassembler = geometry->disassembler();
    disassembler->disassemble(dto);
    geometry->disassemble(disassembler);
    return geometry;
}
#include "TerrainMaker.h"
#include "Terrain/TerrainGeometry.h"
#include "Terrain/TerrainAssemblers.h"
#include "Geometries/GeometryDataQueries.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "Primitives/PrimitiveQueries.h"
#include "Primitives/PrimitiveCommands.h"
#include <memory>

using namespace Enigma::Terrain;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

Enigma::Engine::GenericDto TerrainMaker::makeTerrainGeometry(const Enigma::Geometries::GeometryId& id)
{
    TerrainGeometryAssembler assembler(id);
    assembler.numRows(10).numCols(10).minPosition({ -5.0f, 0.0f, -5.0f }).maxPosition({ 5.0f, 0.0f, 5.0f }).minTextureCoordinate({ 0.0f, 0.0f }).maxTextureCoordinate({ 1.0f, 1.0f });
    assembler.asAsset(id.name(), id.name() + ".geo", "DataPath");
    return assembler.toGenericDto();
}

std::shared_ptr<Enigma::Terrain::TerrainPrimitive> TerrainMaker::makeTerrainPrimitive(const Enigma::Primitives::PrimitiveId& id, const Enigma::Geometries::GeometryId& geo_id, const Enigma::Engine::TextureId& splat_tex_id)
{
    TerrainPrimitiveAssembler assembler(id);
    assembler.meshPrimitive().geometryId(geo_id).effect(EffectMaterialId("fx/TerrainMesh")).textureMap(EffectTextureMapAssembler().textureMapping(TextureId("image/du011"), std::nullopt, "TextureLayer0")
        .textureMapping(TextureId("image/one"), std::nullopt, "TextureLayer1")
        .textureMapping(TextureId("image/two"), std::nullopt, "TextureLayer2")
        .textureMapping(TextureId("image/three"), std::nullopt, "TextureLayer3")
        .textureMapping(splat_tex_id, std::nullopt, "AlphaLayer"));
    assembler.asNative(id.name() + ".terrain@DataPath");
    auto prim = std::make_shared<Enigma::Primitives::RequestPrimitiveConstitution>(id, assembler.toGenericDto())->dispatch();
    std::make_shared<Enigma::Primitives::PutPrimitive>(id, prim)->execute();
    return std::dynamic_pointer_cast<TerrainPrimitive>(prim);
}

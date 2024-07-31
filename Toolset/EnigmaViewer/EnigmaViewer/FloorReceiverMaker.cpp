#include "FloorReceiverMaker.h"
#include "Geometries/GeometryId.h"
#include "GeometryCreationHelper.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/SpatialId.h"
#include "SceneGraph/SceneGraphAssemblers.h"
#include "Renderables/RenderablePrimitiveAssembler.h"
#include "Renderables/RenderablePrimitiveDtos.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "Primitives/PrimitiveId.h"
#include "Renderables/MeshPrimitive.h"
#include "Primitives/PrimitiveQueries.h"
#include "ShadowMap/SpatialShadowFlags.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace EnigmaViewer;
using namespace Enigma::Geometries;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::Primitives;
using namespace Enigma::MathLib;
using namespace Enigma::ShadowMap;

std::string FloorReceiverName = "_Floor_";

void FloorReceiverMaker::makeFloorReceiver(const std::shared_ptr<Enigma::SceneGraph::Node>& root)
{
    auto floorGeometryId = GeometryId("floor");
    auto geometry = GeometryCreationHelper::createSquareXZQuad(floorGeometryId);
    auto floorPawnId = SpatialId(FloorReceiverName, Pawn::TYPE_RTTI);
    PawnAssembler pawn_assembler(floorPawnId);
    MeshPrimitiveDto mesh_dto;
    EffectTextureMapAssembler tex_assembler;
    tex_assembler.textureMapping(TextureId("image/du011"), std::nullopt, "DiffuseMap");
    auto floorMeshId = PrimitiveId("floor_mesh", MeshPrimitive::TYPE_RTTI);
    mesh_dto.id() = floorMeshId;
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(floorMeshId.name() + ".mesh@DataPath");
    mesh_dto.effects().emplace_back(EffectMaterialId("fx/default_textured_mesh_effect"));
    mesh_dto.textureMaps().emplace_back(tex_assembler.toGenericDto());
    mesh_dto.geometryId() = floorGeometryId;
    mesh_dto.renderListID() = Enigma::Renderer::Renderer::RenderListID::Scene;
    mesh_dto.visualTechniqueSelection() = "Default";

    auto mesh = std::make_shared<RequestPrimitiveConstitution>(floorMeshId, mesh_dto.toGenericDto())->dispatch();
    pawn_assembler.primitive(floorMeshId);
    pawn_assembler.spatial().localTransform(Matrix4::IDENTITY).topLevel(true).spatialFlags(SpatialShadowFlags::Spatial_ShadowReceiver);
    auto floor = std::dynamic_pointer_cast<Pawn>(std::make_shared<RequestSpatialConstitution>(floorPawnId, pawn_assembler.toGenericDto())->dispatch());
    if (root) root->attachChild(floor, Matrix4::IDENTITY);
}

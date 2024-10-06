#include "FloorReceiverMaker.h"
#include "Geometries/GeometryId.h"
#include "GeometryCreationHelper.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/SpatialId.h"
#include "SceneGraph/PawnAssembler.h"
#include "Renderables/MeshPrimitiveAssembler.h"
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
    auto floorMeshId = PrimitiveId("floor_mesh", MeshPrimitive::TYPE_RTTI);
    MeshPrimitiveAssembler mesh_assembler(floorMeshId);
    mesh_assembler.addMaterial(std::make_shared<PrimitiveMaterial>(EffectMaterialId("fx/default_textured_mesh_effect"), EffectTextureMap({ {TextureId("image/du011"), std::nullopt, "DiffuseMap"} })));
    mesh_assembler.geometryId(floorGeometryId);
    mesh_assembler.renderListID(Enigma::Renderer::Renderer::RenderListID::Scene);
    mesh_assembler.visualTechnique("Default");
    mesh_assembler.asNative(floorMeshId.name() + ".mesh@DataPath");
    auto mesh = std::make_shared<RequestPrimitiveConstitution>(floorMeshId, mesh_assembler.assemble())->dispatch();
    pawn_assembler.primitiveId(floorMeshId);
    pawn_assembler.localTransform(Matrix4::IDENTITY);
    pawn_assembler.topLevel(true);
    pawn_assembler.spatialFlags(SpatialShadowFlags::Spatial_ShadowReceiver);
    auto floor = std::dynamic_pointer_cast<Pawn>(std::make_shared<RequestSpatialConstitution>(floorPawnId, pawn_assembler.assemble())->dispatch());
    if (root) root->attachChild(floor, Matrix4::IDENTITY);
}

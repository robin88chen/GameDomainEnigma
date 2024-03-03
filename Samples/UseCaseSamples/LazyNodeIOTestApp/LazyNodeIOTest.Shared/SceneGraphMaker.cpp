#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "SceneGraph/LazyNode.h"
#include "SceneGraph/Pawn.h"
#include "SkinMeshModelMaker.h"
#include "CubeGeometryMaker.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "Platforms/PlatformLayer.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/SceneGraphAssemblers.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Renderables;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

void SceneGraphMaker::makePawm(const Enigma::SceneGraph::SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id)
{
    auto pawn = std::dynamic_pointer_cast<Pawn>(std::make_shared<QuerySpatial>(id)->dispatch());
    if (!pawn)
    {
        pawn = PawnAssembler(id).primitive(primitive_id).localTransform(Matrix4::IDENTITY).modelBound(CubeGeometryMaker::getGeometryBound()).topLevel(true).constitute(Enigma::SceneGraph::PersistenceLevel::Store);
    }
}

void SceneGraphMaker::makeStoredLazyNode(const SpatialId& id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    LazyNodeDto lazy_dto;
    lazy_dto.id() = id;
    lazy_dto.isTopLevel() = true;
    lazy_dto.factoryDesc() = FactoryDesc(LazyNode::TYPE_RTTI.getName());
    lazy_dto.name() = id.name();
    lazy_dto.localTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    lazy_dto.worldTransform() = lazy_dto.localTransform();
    lazy_dto.modelBound() = unit_bv.serializeDto().toGenericDto();
    lazy_dto.worldBound() = BoundingVolume::CreateFromTransform(unit_bv, lazy_dto.worldTransform()).serializeDto().toGenericDto();
    lazy_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    lazy_dto.childIds() = children;
    std::make_shared<RequestSpatialConstitution>(id, lazy_dto.toGenericDto(), PersistenceLevel::Store)->dispatch();
}

std::vector<GenericDto> SceneGraphMaker::MakeSceneGraphDtos(const std::string& lazy_node_name)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    NodeDto root_dto;
    root_dto.TheFactoryDesc() = FactoryDesc(Node::TYPE_RTTI.GetName());
    root_dto.Name() = "scene_root";
    root_dto.LocalTransform() = Matrix4::IDENTITY;
    root_dto.WorldTransform() = Matrix4::IDENTITY;
    root_dto.ModelBound() = root_bv.SerializeDto().ToGenericDto();
    root_dto.WorldBound() = root_bv.SerializeDto().ToGenericDto();
    root_dto.IsTopLevel() = true;
    root_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    root_dto.ChildNames() = { "child1", lazy_node_name };
    BoundingVolume child_bv(Box3::UNIT_BOX);
    NodeDto child1_dto;
    child1_dto.TheFactoryDesc() = FactoryDesc(Node::TYPE_RTTI.GetName());
    child1_dto.Name() = "child1";
    child1_dto.LocalTransform() = Matrix4::IDENTITY;
    child1_dto.WorldTransform() = root_dto.WorldTransform() * child1_dto.LocalTransform();
    child1_dto.ModelBound() = child_bv.SerializeDto().ToGenericDto();
    child1_dto.WorldBound() = BoundingVolume::CreateFromTransform(child_bv, child1_dto.WorldTransform()).SerializeDto().ToGenericDto();
    child1_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    LazyNodeDto lazy_dto;
    lazy_dto.TheFactoryDesc() = FactoryDesc(LazyNode::TYPE_RTTI.GetName());
    lazy_dto.TheFactoryDesc().ClaimAsDeferred(lazy_node_name + ".node", "DataPath");
    lazy_dto.Name() = lazy_node_name;
    lazy_dto.LocalTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    lazy_dto.WorldTransform() = root_dto.WorldTransform() * lazy_dto.LocalTransform();
    lazy_dto.ModelBound() = child_bv.SerializeDto().ToGenericDto();
    lazy_dto.WorldBound() = BoundingVolume::CreateFromTransform(child_bv, lazy_dto.WorldTransform()).SerializeDto().ToGenericDto();
    lazy_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);

    std::vector<GenericDto> dtos = { root_dto.ToGenericDto(), child1_dto.ToGenericDto(), lazy_dto.ToGenericDto() };
    std::string json = DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename("scene_root.node@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return dtos;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
    return dtos;
}

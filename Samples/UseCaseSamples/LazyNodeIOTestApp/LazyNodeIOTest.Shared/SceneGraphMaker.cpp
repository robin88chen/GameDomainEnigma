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
        pawn = PawnAssembler(id).primitive(primitive_id).localTransform(Matrix4::IDENTITY).modelBound(CubeGeometryMaker::getGeometryBound()).topLevel(true).asNative(id.name() + ".pawn@DataPath").constitute(Enigma::SceneGraph::PersistenceLevel::Store);
    }
}

void SceneGraphMaker::makeStoredLazyNode(const SpatialId& id, const SpatialId& parent_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    LazyNodeDto lazy_dto;
    lazy_dto.id() = id;
    lazy_dto.isTopLevel() = true;
    lazy_dto.factoryDesc() = FactoryDesc(LazyNode::TYPE_RTTI.getName()).ClaimAsDeferred(id.name() + ".node", "DataPath");
    lazy_dto.name() = id.name();
    lazy_dto.localTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    lazy_dto.worldTransform() = lazy_dto.localTransform();
    lazy_dto.modelBound() = unit_bv.serializeDto().toGenericDto();
    lazy_dto.worldBound() = BoundingVolume::CreateFromTransform(unit_bv, lazy_dto.worldTransform()).serializeDto().toGenericDto();
    lazy_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    for (const auto& child : children)
    {
        lazy_dto.children().push_back(NodeDto::ChildDto{ child, GenericDto() });
    }
    lazy_dto.parentId() = parent_id;
    std::make_shared<RequestSpatialConstitution>(id, lazy_dto.toGenericDto(), PersistenceLevel::Store)->dispatch();
}

GenericDto SceneGraphMaker::makeSceneGraph(const SpatialId& root_id, const SpatialId& lazy_node_id)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    NodeDto root_dto;
    root_dto.id() = root_id;
    root_dto.name() = root_id.name();
    root_dto.factoryDesc() = FactoryDesc(Node::TYPE_RTTI.getName()).ClaimAsNative(root_id.name() + ".node@DataPath");
    root_dto.localTransform() = Matrix4::IDENTITY;
    root_dto.worldTransform() = Matrix4::IDENTITY;
    root_dto.modelBound() = root_bv.serializeDto().toGenericDto();
    root_dto.worldBound() = root_bv.serializeDto().toGenericDto();
    root_dto.isTopLevel() = true;
    root_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    //root_dto.ChildNames() = { "child1", lazy_node_name };
    BoundingVolume child_bv(Box3::UNIT_BOX);
    NodeDto child1_dto;
    child1_dto.id() = SpatialId("child1", Node::TYPE_RTTI);
    child1_dto.name() = "child1";
    child1_dto.localTransform() = Matrix4::IDENTITY;
    child1_dto.worldTransform() = root_dto.worldTransform() * child1_dto.localTransform();
    child1_dto.modelBound() = child_bv.serializeDto().toGenericDto();
    child1_dto.worldBound() = BoundingVolume::CreateFromTransform(child_bv, child1_dto.worldTransform()).serializeDto().toGenericDto();
    child1_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    child1_dto.parentId() = root_dto.id();
    LazyNodeDto lazy_dto;
    lazy_dto.id() = lazy_node_id;
    lazy_dto.factoryDesc() = FactoryDesc(LazyNode::TYPE_RTTI.getName()).ClaimAsInstanced(lazy_node_id.name() + ".node", "DataPath");
    lazy_dto.name() = lazy_node_id.name();
    lazy_dto.localTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    lazy_dto.worldTransform() = root_dto.worldTransform() * lazy_dto.localTransform();
    lazy_dto.modelBound() = child_bv.serializeDto().toGenericDto();
    lazy_dto.worldBound() = BoundingVolume::CreateFromTransform(child_bv, lazy_dto.worldTransform()).serializeDto().toGenericDto();
    lazy_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    lazy_dto.parentId() = root_id;
    root_dto.children().emplace_back(NodeDto::ChildDto{ child1_dto.id(), child1_dto.toGenericDto() });
    root_dto.children().emplace_back(NodeDto::ChildDto{ lazy_node_id, lazy_dto.toGenericDto() });
    return root_dto.toGenericDto();
}

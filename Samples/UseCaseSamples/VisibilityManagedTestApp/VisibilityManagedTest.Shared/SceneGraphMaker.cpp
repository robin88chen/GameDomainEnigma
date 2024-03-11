#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "SceneGraph/Pawn.h"
#include "CubeGeometryMaker.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/SceneGraphAssemblers.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
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

GenericDto SceneGraphMaker::makeVisibilityNode(const SpatialId& id, const SpatialId& parent_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    VisibilityManagedNodeDto visibility_dto;
    visibility_dto.id() = id;
    visibility_dto.isTopLevel() = true;
    visibility_dto.factoryDesc() = FactoryDesc(VisibilityManagedNode::TYPE_RTTI.getName()).ClaimAsDeferred(id.name() + ".node", "DataPath");
    visibility_dto.name() = id.name();
    visibility_dto.localTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    visibility_dto.worldTransform() = visibility_dto.localTransform();
    visibility_dto.modelBound() = unit_bv.serializeDto().toGenericDto();
    visibility_dto.worldBound() = BoundingVolume::CreateFromTransform(unit_bv, visibility_dto.worldTransform()).serializeDto().toGenericDto();
    visibility_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    for (const auto& child : children)
    {
        visibility_dto.children().push_back(child);
    }
    visibility_dto.parentId() = parent_id;
    return visibility_dto.toGenericDto();
}

GenericDto SceneGraphMaker::makeSceneGraph(const SpatialId& root_id, const SpatialId& visibility_node_id)
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
    VisibilityManagedNodeDto visibility_dto;
    visibility_dto.id() = visibility_node_id;
    visibility_dto.factoryDesc() = FactoryDesc(VisibilityManagedNode::TYPE_RTTI.getName()).ClaimAsInstanced(visibility_node_id.name() + ".node", "DataPath");
    visibility_dto.name() = visibility_node_id.name();
    visibility_dto.localTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    visibility_dto.worldTransform() = root_dto.worldTransform() * visibility_dto.localTransform();
    visibility_dto.modelBound() = child_bv.serializeDto().toGenericDto();
    visibility_dto.worldBound() = BoundingVolume::CreateFromTransform(child_bv, visibility_dto.worldTransform()).serializeDto().toGenericDto();
    visibility_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    visibility_dto.parentId() = root_id;
    root_dto.children().emplace_back(NodeDto::ChildDto{ child1_dto.id(), child1_dto.toGenericDto() });
    root_dto.children().emplace_back(NodeDto::ChildDto{ visibility_node_id, visibility_dto.toGenericDto() });
    return root_dto.toGenericDto();
}

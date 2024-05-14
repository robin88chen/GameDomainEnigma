#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphAssemblers.h"
#include "MathLib/Matrix4.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/SceneGraphStoreMapper.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

void SceneGraphMaker::makeOneLevelQuadNode(const SpatialId& id, const std::shared_ptr<SceneGraphStoreMapper>& store_mapper)
{
    Box3 box = Box3::UNIT_BOX;
    box.Extent(0) = 8.0f;
    box.Extent(1) = 8.0f;
    box.Extent(2) = 8.0f;
    VisibilityManagedNodeAssembler node_assembler(id);
    node_assembler.lazyNode().node().spatial().localTransform(Matrix4::IDENTITY).modelBound(BoundingVolume(box));
    node_assembler.asDeferred(id.name() + ".quad", "DataPath");
    store_mapper->putSpatial(id, node_assembler.toDeHydratedGenericDto());
    store_mapper->putLaziedContent(id, node_assembler.toHydratedGenericDto());
}

void SceneGraphMaker::makeThreeLevelQuadTree(const SpatialId& root_id, const SpatialId& level1_id1, const SpatialId& level1_id2, const SpatialId& level2_id1, const std::shared_ptr<SceneGraphStoreMapper>& store_mapper)
{
    VisibilityManagedNodeAssembler root_assembler(root_id);
    root_assembler.lazyNode().node().spatial().localTransform(Matrix4::IDENTITY).modelBound(BoundingVolume(Box3::UNIT_BOX));
    root_assembler.asDeferred(root_id.name() + ".quad", "DataPath");

    Box3 level1_box = Box3(Vector3::ZERO, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, 4.0f, 4.0f, 4.0f);
    VisibilityManagedNodeAssembler level1_assembler1(level1_id1);
    level1_assembler1.lazyNode().node().spatial().localTransform(Matrix4::MakeTranslateTransform(Vector3(-4.0f, 0.0f, -4.0f))).modelBound(BoundingVolume(level1_box));
    level1_assembler1.asDeferred(level1_id1.name() + ".quad", "DataPath");
    VisibilityManagedNodeAssembler level1_assembler2(level1_id2);
    level1_assembler2.lazyNode().node().spatial().localTransform(Matrix4::MakeTranslateTransform(Vector3(4.0f, 0.0f, 4.0f))).modelBound(BoundingVolume(level1_box));
    level1_assembler2.asDeferred(level1_id2.name() + ".quad", "DataPath");

    Box3 level2_box = Box3(Vector3::ZERO, Vector3::UNIT_X, Vector3::UNIT_Y, Vector3::UNIT_Z, 2.0f, 2.0f, 2.0f);
    VisibilityManagedNodeAssembler level2_assembler(level2_id1);
    level2_assembler.lazyNode().node().spatial().localTransform(Matrix4::MakeTranslateTransform(Vector3(-2.0f, 0.0f, -2.0f))).modelBound(BoundingVolume(level2_box));
    level2_assembler.asDeferred(level2_id1.name() + ".quad", "DataPath");

    root_assembler.lazyNode().node().child(level1_id1, &(level1_assembler1.lazyNode().node())).child(level1_id2, &(level1_assembler2).lazyNode().node());
    level1_assembler2.lazyNode().node().child(level2_id1, &(level2_assembler.lazyNode().node()));

    store_mapper->putSpatial(root_id, root_assembler.toHydratedGenericDto());
    store_mapper->putSpatial(level1_id1, level1_assembler1.toHydratedGenericDto());
    store_mapper->putSpatial(level1_id2, level1_assembler2.toHydratedGenericDto());
    store_mapper->putSpatial(level2_id1, level2_assembler.toHydratedGenericDto());
}

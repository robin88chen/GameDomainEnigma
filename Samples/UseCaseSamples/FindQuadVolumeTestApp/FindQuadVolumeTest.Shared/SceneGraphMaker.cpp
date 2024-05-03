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
    store_mapper->putSpatial(id, node_assembler.toHydratedGenericDto());
}

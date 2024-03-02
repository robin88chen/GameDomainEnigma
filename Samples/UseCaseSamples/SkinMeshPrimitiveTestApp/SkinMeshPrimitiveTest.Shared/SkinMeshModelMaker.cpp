#include "SkinMeshModelMaker.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/SkinMeshPrimitive.h"
#include "GameEngine/EffectDtoHelper.h"
#include "Primitives/PrimitiveId.h"
#include "Geometries/GeometryId.h"
#include "Renderables/RenderablePrimitiveAssembler.h"

using namespace Enigma::Engine;
using namespace Enigma::Animators;
using namespace Enigma::Renderables;
using namespace Enigma::MathLib;
using namespace Enigma::Primitives;

std::shared_ptr<MeshPrimitive> SkinMeshModelMaker::makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id)
{
    if (auto mesh = Primitive::queryPrimitive(mesh_id)) return std::dynamic_pointer_cast<MeshPrimitive>(mesh);
    return SkinMeshPrimitiveAssembler(mesh_id).geometryId(geo_id).asNative(mesh_id.name() + ".mesh@DataPath").effect(EffectMaterialId("skin_mesh_prim_test")).textureMap(EffectTextureMapDtoHelper().textureMapping(TextureId("earth"), std::nullopt, "DiffuseMap")).renderListID(Enigma::Renderer::Renderer::RenderListID::Scene).visualTechnique("Default").constitute(PersistenceLevel::Store);
}

std::shared_ptr<ModelPrimitive> SkinMeshModelMaker::makeModelPrimitive(const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Animators::AnimatorId& animator_id, const std::vector<std::string>& mesh_node_names)
{
    if (auto model = Primitive::queryPrimitive(model_id)) return std::dynamic_pointer_cast<ModelPrimitive>(model);

    MeshNodeTreeAssembler tree;
    for (unsigned i = 0; i < mesh_node_names.size(); i++)
    {
        MeshNodeAssembler node(mesh_node_names[i]);
        auto transform = Matrix4::MakeTranslateTransform(Vector3(0.0f, 2.0f * i, 0.0f));
        node.localT_PosTransform(transform);
        if (i != 0)
        {
            node.parentNode(mesh_node_names[i - 1]);
        }
        if (i == 0)
        {
            node.meshPrimitive(mesh_id.next());
        }
        tree.addNode(node);
    }
    return ModelPrimitiveAssembler(model_id).asNative(model_id.name() + ".model@DataPath").animator(animator_id).meshNodeTree(tree).constitute(PersistenceLevel::Store);
}

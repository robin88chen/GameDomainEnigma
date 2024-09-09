#include "SkinMeshModelMaker.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/SkinMeshPrimitive.h"
#include "GameEngine/EffectTextureMapAssembler.h"
#include "Primitives/PrimitiveId.h"
#include "Geometries/GeometryId.h"
#include "Renderables/SkinMeshPrimitiveAssembler.h"
#include "Primitives/PrimitiveCommands.h"
#include "Renderables/ModelPrimitiveAssembler.h"
#include "Primitives/PrimitiveQueries.h"
#include "Renderables/MeshNodeAssemblers.h"

using namespace Enigma::Engine;
using namespace Enigma::Animators;
using namespace Enigma::Renderables;
using namespace Enigma::MathLib;
using namespace Enigma::Primitives;

std::shared_ptr<MeshPrimitive> SkinMeshModelMaker::makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id)
{
    if (auto mesh = Primitive::queryPrimitive(mesh_id)) return std::dynamic_pointer_cast<MeshPrimitive>(mesh);
    SkinMeshPrimitiveAssembler assembler(mesh_id);
    assembler.geometryId(geo_id);
    assembler.asNative(mesh_id.name() + ".mesh@DataPath");
    assembler.addEffect(EffectMaterialId("skin_mesh_prim_test"));
    std::shared_ptr<EffectTextureMapAssembler> texture_assembler = std::make_shared<EffectTextureMapAssembler>();
    texture_assembler->addTextureMapping(TextureId("earth"), std::nullopt, "DiffuseMap");
    assembler.addTextureMap(texture_assembler);
    assembler.renderListID(Enigma::Renderer::Renderer::RenderListID::Scene);
    assembler.visualTechnique("Default");
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(std::make_shared<RequestPrimitiveConstitution>(mesh_id, assembler.assemble())->dispatch());
    std::make_shared<PutPrimitive>(mesh_id, mesh)->execute();
    return mesh;
}

std::shared_ptr<ModelPrimitive> SkinMeshModelMaker::makeModelPrimitive(const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Animators::AnimatorId& animator_id, const std::vector<std::string>& mesh_node_names)
{
    if (auto model = Primitive::queryPrimitive(model_id)) return std::dynamic_pointer_cast<ModelPrimitive>(model);

    std::shared_ptr<MeshNodeTreeAssembler> tree = std::make_shared<MeshNodeTreeAssembler>();
    for (unsigned i = 0; i < mesh_node_names.size(); i++)
    {
        std::shared_ptr<MeshNodeAssembler> node_assembler = std::make_shared<MeshNodeAssembler>();
        node_assembler->name(mesh_node_names[i]);
        auto transform = Matrix4::MakeTranslateTransform(Vector3(0.0f, 2.0f * i, 0.0f));
        node_assembler->localT_PosTransform(transform);
        if (i != 0)
        {
            tree->addNodeWithParentName(mesh_node_names[i], node_assembler, mesh_node_names[i - 1]);
        }
        if (i == 0)
        {
            node_assembler->meshPrimitiveId(mesh_id);
            tree->addNode(mesh_node_names[i], node_assembler);
        }
    }
    ModelPrimitiveAssembler model_assembler(model_id);
    model_assembler.meshNodeTree(tree);
    model_assembler.asNative(model_id.name() + ".model@DataPath");
    model_assembler.modelAnimatorId(animator_id);
    auto model = std::dynamic_pointer_cast<ModelPrimitive>(std::make_shared<RequestPrimitiveConstitution>(model_id, model_assembler.assemble())->dispatch());
    std::make_shared<PutPrimitive>(model_id, model)->execute();
    return model;
}

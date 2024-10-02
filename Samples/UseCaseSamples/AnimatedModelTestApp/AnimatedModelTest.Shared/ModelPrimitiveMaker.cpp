#include "ModelPrimitiveMaker.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/MeshNodeAssemblers.h"
#include "Renderables/MeshPrimitive.h"
#include "Renderables/ModelPrimitiveAssembler.h"
#include "Renderables/MeshPrimitiveAssembler.h"
#include "Primitives/PrimitiveQueries.h"

using namespace Enigma::Engine;
using namespace Enigma::Renderables;
using namespace Enigma::MathLib;
using namespace Enigma::Frameworks;
using namespace Enigma::Primitives;

std::shared_ptr<ModelPrimitive> ModelPrimitiveMaker::makeModelPrimitive(const PrimitiveId& model_id, const PrimitiveId& mesh_id, const Enigma::Animators::AnimatorId& animator_id, const std::vector<std::string>& mesh_node_names)
{
    if (auto model = Primitive::queryPrimitive(model_id))
    {
        return std::dynamic_pointer_cast<ModelPrimitive>(model);
    }
    std::shared_ptr<MeshNodeTreeAssembler> tree = std::make_shared<MeshNodeTreeAssembler>();
    for (unsigned i = 0; i < 4; i++)
    {
        std::shared_ptr<MeshNodeAssembler> node_assembler = std::make_shared<MeshNodeAssembler>();
        node_assembler->name(mesh_node_names[i]);
        node_assembler->localT_PosTransform(Matrix4::MakeTranslateTransform((float)i * 0.5f, (float)i * 0.3f, (float)i * 0.2f));
        if (i != 0)
        {
            node_assembler->parentIndexInArray((unsigned)(i - 1));
        }
        if ((i == 0) || (i == 3))
        {
            node_assembler->meshPrimitiveId(mesh_id);
        }
        tree->addNode(mesh_node_names[i], node_assembler);
    }
    ModelPrimitiveAssembler model_assembler(model_id);
    model_assembler.meshNodeTree(tree);
    model_assembler.asNative(model_id.name() + ".model@DataPath");
    model_assembler.modelAnimatorId(animator_id);
    return std::dynamic_pointer_cast<ModelPrimitive>(std::make_shared<RequestPrimitiveConstitution>(model_id, model_assembler.assemble())->dispatch());
}

std::shared_ptr<MeshPrimitive> ModelPrimitiveMaker::makeCubeMeshPrimitive(const PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id)
{
    if (auto mesh = Primitive::queryPrimitive(mesh_id))
    {
        return std::dynamic_pointer_cast<MeshPrimitive>(mesh);
    };
    MeshPrimitiveAssembler mesh_assembler(mesh_id);
    mesh_assembler.geometryId(geo_id);
    mesh_assembler.asNative(mesh_id.name() + ".mesh@DataPath");
    mesh_assembler.addMaterial(std::make_shared<PrimitiveMaterial>(EffectMaterialId("basic_vtx_tex"), EffectTextureMap({ {TextureId("earth"), std::nullopt, "DiffuseMap"} })));
    mesh_assembler.renderListID(Enigma::Renderer::Renderer::RenderListID::Scene);
    mesh_assembler.visualTechnique("Default");

    return std::dynamic_pointer_cast<MeshPrimitive>(std::make_shared<RequestPrimitiveConstitution>(mesh_id, mesh_assembler.assemble())->dispatch());
}

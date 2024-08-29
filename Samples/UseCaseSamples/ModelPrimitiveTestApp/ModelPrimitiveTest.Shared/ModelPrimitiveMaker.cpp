#include "ModelPrimitiveMaker.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/ModelPrimitiveAssembler.h"
#include "Renderables/MeshNodeAssemblers.h"
#include "Frameworks/StringFormat.h"
#include "Renderables/MeshPrimitive.h"
#include "Renderables/MeshPrimitiveAssembler.h"
#include "Primitives/PrimitiveQueries.h"

using namespace Enigma::Engine;
using namespace Enigma::Renderer;
using namespace Enigma::MathLib;
using namespace Enigma::Frameworks;
using namespace Enigma::Primitives;
using namespace Enigma::Geometries;
using namespace Enigma::Renderables;

std::shared_ptr<ModelPrimitive> ModelPrimitiveMaker::makeModelPrimitive(const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& mesh_id)
{
    if (auto model = Primitive::queryPrimitive(model_id))
    {
        return std::dynamic_pointer_cast<ModelPrimitive>(model);
    }
    std::shared_ptr<MeshNodeTreeAssembler> tree = std::make_shared<MeshNodeTreeAssembler>();
    for (unsigned i = 0; i < 4; i++)
    {
        std::string name = string_format("%s_node_%d", model_id.name().c_str(), i);
        std::shared_ptr<MeshNodeAssembler> node_assembler = std::make_shared<MeshNodeAssembler>();
        node_assembler->name(name);
        node_assembler->localT_PosTransform(Matrix4::MakeTranslateTransform((float)i * 0.5f, (float)i * 0.3f, (float)i * 0.2f));
        if (i != 0)
        {
            node_assembler->parentIndexInArray((unsigned)(i - 1));
        }
        if ((i == 0) || (i == 3))
        {
            node_assembler->meshPrimitiveId(mesh_id);
        }
        tree->addNode(name, node_assembler);
    }
    ModelPrimitiveAssembler model_assembler(model_id);
    model_assembler.meshNodeTree(tree);
    model_assembler.asNative(model_id.name() + ".model@DataPath");
    return std::dynamic_pointer_cast<ModelPrimitive>(std::make_shared<RequestPrimitiveConstitution>(model_id, model_assembler.assemble())->dispatch());
}

std::shared_ptr<MeshPrimitive> ModelPrimitiveMaker::makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id)
{
    if (auto mesh = Primitive::queryPrimitive(mesh_id))
    {
        return std::dynamic_pointer_cast<MeshPrimitive>(mesh);
    };
    MeshPrimitiveAssembler mesh_assembler(mesh_id);
    mesh_assembler.geometryId(geo_id);
    mesh_assembler.asNative(mesh_id.name() + ".mesh@DataPath");
    std::shared_ptr<EffectTextureMapAssembler> texture_assembler = std::make_shared<EffectTextureMapAssembler>();
    texture_assembler->addTextureMapping(TextureId("earth"), std::nullopt, "DiffuseMap");
    mesh_assembler.addEffect(EffectMaterialId("basic_vtx_tex"));
    mesh_assembler.addTextureMap(texture_assembler);
    mesh_assembler.renderListID(Renderer::RenderListID::Scene);
    mesh_assembler.visualTechnique("Default");

    return std::dynamic_pointer_cast<MeshPrimitive>(std::make_shared<RequestPrimitiveConstitution>(mesh_id, mesh_assembler.assemble())->dispatch());
}

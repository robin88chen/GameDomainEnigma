#include "MeshPrimitiveMaker.h"
#include "Renderables/MeshPrimitive.h"
#include "Renderables/MeshPrimitiveAssembler.h"
#include "Primitives/PrimitiveQueries.h"

using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::Primitives;
using namespace Enigma::Renderer;

std::shared_ptr<MeshPrimitive> MeshPrimitiveMaker::makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id)
{
    if (const auto mesh = Primitive::queryPrimitive(mesh_id))
    {
        return std::dynamic_pointer_cast<MeshPrimitive>(mesh);
    }
    MeshPrimitiveAssembler assembler(mesh_id);
    assembler.geometryId(geo_id);
    assembler.asNative(mesh_id.name() + ".mesh@DataPath");
    assembler.addMaterial(std::make_shared<PrimitiveMaterial>(EffectMaterialId("basic_vtx_tex"), EffectTextureMap({ { TextureId("earth"), std::nullopt, "DiffuseMap" } })));
    assembler.renderListID(Renderer::RenderListID::Scene);
    assembler.visualTechnique("Default");

    return std::dynamic_pointer_cast<MeshPrimitive>(std::make_shared<RequestPrimitiveConstitution>(mesh_id, assembler.assemble())->dispatch());
}

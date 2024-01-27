#include "ModelPrimitiveMaker.h"
#include "Renderables/ModelPrimitive.h"
#include "Renderables/RenderablePrimitiveDtos.h"
#include "Frameworks/StringFormat.h"
#include "Renderables/MeshPrimitive.h"
#include "GameEngine/EffectDtoHelper.h"
#include "Frameworks/CommandBus.h"
#include "Primitives/PrimitiveCommands.h"

using namespace Enigma::Engine;
using namespace Enigma::Renderables;
using namespace Enigma::MathLib;
using namespace Enigma::Frameworks;

void ModelPrimitiveMaker::makeModelPrimitive(const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Animators::AnimatorId& animator_id, const std::vector<std::string>& mesh_node_names)
{
    MeshNodeTreeDto tree;
    for (unsigned i = 0; i < 4; i++)
    {
        MeshNodeDto node;
        node.name() = mesh_node_names[i];
        node.localT_PosTransform() = Matrix4::MakeTranslateTransform((float)i * 0.5f, (float)i * 0.3f, (float)i * 0.2f);
        if (i != 0)
        {
            node.parentIndexInArray() = (unsigned)(i - 1);
        }
        if ((i == 0) || (i == 3))
        {
            node.meshPrimitiveId() = mesh_id.next();
        }
        tree.meshNodes().emplace_back(node.toGenericDto());
    }
    ModelPrimitiveDto model_dto;
    model_dto.id() = model_id;
    model_dto.factoryDesc() = FactoryDesc(ModelPrimitive::TYPE_RTTI.getName()).ClaimAsNative(model_id.name() + ".model@DataPath");
    model_dto.nodeTree() = tree.toGenericDto();
    model_dto.animatorId() = animator_id;
    Enigma::Frameworks::CommandBus::post(std::make_shared<Enigma::Primitives::ConstitutePrimitive>(model_id, model_dto.toGenericDto()));
}

void ModelPrimitiveMaker::makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id)
{
    MeshPrimitiveDto mesh_dto;
    mesh_dto.id() = mesh_id;
    mesh_dto.geometryId() = geo_id;
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(mesh_id.name() + ".mesh@DataPath");
    mesh_dto.effects().emplace_back(EffectMaterialId("basic_vtx_tex"));
    EffectTextureMapDtoHelper texture_helper;
    texture_helper.textureMapping(TextureId("earth"), std::nullopt, "DiffuseMap");
    mesh_dto.textureMaps().emplace_back(texture_helper.toGenericDto());
    mesh_dto.renderListID() = Enigma::Renderer::Renderer::RenderListID::Scene;
    mesh_dto.visualTechniqueSelection() = "Default";

    Enigma::Frameworks::CommandBus::post(std::make_shared<Enigma::Primitives::ConstitutePrimitive>(mesh_id, mesh_dto.toGenericDto()));
}

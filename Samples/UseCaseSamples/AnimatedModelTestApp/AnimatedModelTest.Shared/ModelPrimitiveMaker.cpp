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

void ModelPrimitiveMaker::makeModelPrimitive(const Enigma::Primitives::PrimitiveId& model_id, const Enigma::Primitives::PrimitiveId& mesh_id, const std::vector<std::string>& mesh_node_names)
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

/*std::shared_ptr<ModelPrimitivePolicy> ModelPrimitiveMaker::MakeModelPrimitivePolicy(
    const std::string& model_name, const std::string& geo_name)
{
    MeshNodeTreeDto tree;
    std::vector<std::string> node_names;
    for (unsigned i = 0; i < 4; i++)
    {
        MeshNodeDto node;
        node.Name() = string_format("%s_node_%d", model_name.c_str(), i);
        node_names.emplace_back(node.Name());
        node.LocalTransform() = Matrix4::MakeTranslateTransform(0.0f, (float)i * 0.3f, 0.0f);
        if (i != 0)
        {
            node.ParentIndexInArray() = (unsigned)(i - 1);
        }
        if ((i == 0) || (i == 3))
        {
            node.TheMeshPrimitive() = MakeMeshPrimitiveDto(model_name + "_mesh", geo_name).ToGenericDto();
        }
        tree.MeshNodes().emplace_back(node.ToGenericDto());
    }
    ModelPrimitiveDto dto;
    dto.Name() = model_name;
    dto.TheNodeTree() = tree.ToGenericDto();
    auto animation = ModelAnimatorMaker::MakeModelAnimationAsset(model_name, node_names);
    auto animator = ModelAnimatorMaker::MakeModelAnimator(model_name, animation);
    dto.TheAnimator() = animator->SerializeDto().ToGenericDto();
    return dto.ConvertToPolicy(std::make_shared<JsonFileDtoDeserializer>(),
        std::make_shared<JsonFileEffectProfileDeserializer>());
}

void ModelPrimitiveMaker::SaveModelPrimitiveDto(const std::shared_ptr<ModelPrimitive>& model, const std::string& filename_at_path)
{
    unsigned mesh_count = model->GetMeshPrimitiveCount();
    for (unsigned i = 0; i < mesh_count; i++)
    {
        auto mesh = model->GetMeshPrimitive(i);
        mesh->GetGeometryData()->TheFactoryDesc().ClaimFromResource(mesh->GetGeometryData()->GetName(), "test_geometry.geo@DataPath");
    }
    if (auto ani = std::dynamic_pointer_cast<ModelPrimitiveAnimator, Animator>(model->GetAnimator()))
    {
        if (auto asset = ani->GetAnimationAsset())
        {
            asset->TheFactoryDesc().ClaimFromResource(asset->GetName(), asset->GetName() + ".ani@DataPath");
        }
    }
    GenericDto dto = model->SerializeDto();
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto> {dto });
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(filename_at_path), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}

std::shared_ptr<ModelPrimitivePolicy> ModelPrimitiveMaker::LoadModelPrimitivePolicy(const std::string& filename_at_path)
{
    IFilePtr readFile = FileSystem::Instance()->OpenFile(Filename(filename_at_path), "rb");
    size_t filesize = readFile->Size();
    auto read_buff = readFile->Read(0, filesize);
    std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
    std::vector<GenericDto> read_dtos = DtoJsonGateway::Deserialize(read_json);
    assert(read_dtos.size() == 1);
    FileSystem::Instance()->CloseFile(readFile);
    if (read_dtos[0].GetRtti().GetRttiName() != ModelPrimitive::TYPE_RTTI.GetName()) return nullptr;
    return ModelPrimitiveDto::FromGenericDto(read_dtos[0]).ConvertToPolicy(std::make_shared<JsonFileDtoDeserializer>(),
        std::make_shared<JsonFileEffectProfileDeserializer>());
}

MeshPrimitiveDto ModelPrimitiveMaker::MakeMeshPrimitiveDto(const std::string& mesh_name, const std::string& geo_name)
{
    MeshPrimitiveDto dto;
    dto.Name() = mesh_name;
    dto.GeometryName() = geo_name;
    dto.GeometryFactoryDesc() = FactoryDesc(TriangleList::TYPE_RTTI.GetName()).ClaimFromResource(geo_name, geo_name + ".geo", "DataPath");
    dto.Effects().emplace_back(MakeEffectDto("basic_vtx_tex").ToGenericDto());
    dto.TextureMaps().emplace_back(MakeTextureMapDto().ToGenericDto());
    return dto;
}

EffectMaterialDto ModelPrimitiveMaker::MakeEffectDto(const std::string& eff_name)
{
    EffectMaterialDto dto;
    dto.Name() = eff_name;
    dto.TheFactoryDesc() = FactoryDesc(EffectMaterial::TYPE_RTTI.GetName()).ClaimFromResource(eff_name, eff_name + ".efx", "APK_PATH");
    return dto;
}

EffectTextureMapDto ModelPrimitiveMaker::MakeTextureMapDto()
{
    EffectTextureMapDto dto;
    TextureMappingDto tex;
    tex.Filename() = "earth.png";
    tex.Semantic() = "DiffuseMap";
    tex.TextureName() = "earth";
    tex.PathId() = "APK_PATH";
    dto.TextureMappings().emplace_back(tex);
    return dto;
}*/

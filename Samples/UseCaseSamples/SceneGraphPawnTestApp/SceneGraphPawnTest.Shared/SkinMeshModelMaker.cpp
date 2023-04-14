#include "SkinMeshModelMaker.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "Frameworks/StringFormat.h"
#include "Gateways/DtoJsonGateway.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "GameEngine/TriangleList.h"
#include "SkinAnimationMaker.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Renderer;
using namespace Enigma::MathLib;
using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::Animators;
using namespace Enigma::FileSystem;

std::vector<std::string> bone_node_names{ "bone_root", "bone_one" };

std::shared_ptr<ModelPrimitivePolicy> SkinMeshModelMaker::MakeModelPrimitivePolicy(const std::string& model_name, const std::string& geo_name)
{
    return MakeModelPrimitiveDto(model_name, geo_name).ConvertToPolicy(std::make_shared<JsonFileDtoDeserializer>());
}

ModelPrimitiveDto SkinMeshModelMaker::MakeModelPrimitiveDto(const std::string& model_name, const std::string& geo_name)
{
    std::string skinmesh_name = model_name + "_skinmesh";
    MeshNodeTreeDto tree;
    MeshNodeDto root_node_dto;
    root_node_dto.Name() = bone_node_names[0];
    root_node_dto.LocalTransform() = Matrix4::MakeIdentity();
    root_node_dto.TheMeshPrimitive() = MakeSkinMeshPrimitiveDto(model_name + "_skinmesh", geo_name).ToGenericDto();
    tree.MeshNodes().emplace_back(root_node_dto.ToGenericDto());
    MeshNodeDto bone_node_dto;
    bone_node_dto.Name() = bone_node_names[1];
    bone_node_dto.LocalTransform() = Matrix4::MakeTranslateTransform(Vector3(0.0f, 2.0f, 0.0f));
    bone_node_dto.ParentIndexInArray() = static_cast<unsigned>(tree.MeshNodes().size() - 1);
    tree.MeshNodes().emplace_back(bone_node_dto.ToGenericDto());

    ModelPrimitiveDto dto;
    dto.Name() = model_name;
    dto.TheNodeTree() = tree.ToGenericDto();
    auto animation = SkinAnimationMaker::MakeSkinAnimationAsset(model_name, bone_node_names);
    auto animator_dto = SkinAnimationMaker::MakeModelAnimatorDto(animation, skinmesh_name, bone_node_names);
    dto.TheAnimator() = animator_dto.ToGenericDto();
    return dto;
}

void SkinMeshModelMaker::SaveModelPrimitiveDto(const std::shared_ptr<ModelPrimitive>& model, const std::string& filename_at_path)
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

std::shared_ptr<ModelPrimitivePolicy> SkinMeshModelMaker::LoadModelPrimitivePolicy(const std::string& filename_at_path)
{
    IFilePtr readFile = FileSystem::Instance()->OpenFile(Filename(filename_at_path), "rb");
    size_t filesize = readFile->Size();
    auto read_buff = readFile->Read(0, filesize);
    std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
    std::vector<GenericDto> read_dtos = DtoJsonGateway::Deserialize(read_json);
    assert(read_dtos.size() == 1);
    FileSystem::Instance()->CloseFile(readFile);
    if (read_dtos[0].GetRtti().GetRttiName() != ModelPrimitive::TYPE_RTTI.GetName()) return nullptr;
    return ModelPrimitiveDto::FromGenericDto(read_dtos[0]).ConvertToPolicy(std::make_shared<JsonFileDtoDeserializer>());
}

SkinMeshPrimitiveDto SkinMeshModelMaker::MakeSkinMeshPrimitiveDto(const std::string& mesh_name, const std::string& geo_name)
{
    SkinMeshPrimitiveDto dto;
    dto.Name() = mesh_name;
    dto.GeometryName() = geo_name;
    dto.GeometryFactoryDesc() = FactoryDesc(TriangleList::TYPE_RTTI.GetName()).ClaimFromResource(geo_name, geo_name + ".geo", "DataPath");
    dto.Effects().emplace_back(MakeEffectDto("skin_mesh_prim_test").ToGenericDto());
    dto.TextureMaps().emplace_back(MakeTextureMapDto().ToGenericDto());
    return dto;
}

EffectMaterialDto SkinMeshModelMaker::MakeEffectDto(const std::string& eff_name)
{
    EffectMaterialDto dto;
    dto.Name() = eff_name;
    dto.TheFactoryDesc() = FactoryDesc(EffectMaterial::TYPE_RTTI.GetName()).ClaimFromResource(eff_name, eff_name + ".efx", "APK_PATH");
    return dto;
}

EffectTextureMapDto SkinMeshModelMaker::MakeTextureMapDto()
{
    EffectTextureMapDto dto;
    TextureMappingDto tex;
    tex.Filename() = "earth.png";
    tex.Semantic() = "DiffuseMap";
    tex.TextureName() = "earth";
    tex.PathId() = "APK_PATH";
    dto.TextureMappings().emplace_back(tex);
    return dto;
}

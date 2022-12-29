#include "ModelPrimitiveMaker.h"
#include "Renderer/RenderablePrimitivePolicies.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameEngine/TriangleList.h"
#include "Frameworks/StringFormat.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;
using namespace Enigma::MathLib;

std::shared_ptr<ModelPrimitivePolicy> ModelPrimitiveMaker::MakeModelPrimitivePolicy(
    const std::string& model_name, const std::string& geo_name)
{
    MeshNodeTreeDto tree;
    for (unsigned i = 0; i < 4; i++)
    {
        MeshNodeDto node;
        node.Name() = string_format("%s_node_%d", model_name.c_str(), i);
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
}

#include "MeshPrimitiveMaker.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "CubeGeometryMaker.h"
#include "GameEngine/EffectDtoHelper.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/IFile.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "Renderer/MeshPrimitive.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/PrimitiveCommands.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

void MeshPrimitiveMaker::makeCubeMeshPrimitive(const Enigma::Engine::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id)
{
    MeshPrimitiveDto mesh_dto;
    mesh_dto.id() = mesh_id;
    mesh_dto.geometryId() = geo_id;
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName());
    EffectMaterialDtoHelper effect_helper("basic_vtx_tex");
    effect_helper.FilenameAtPath("basic_vtx_tex.efx", "APK_PATH");
    mesh_dto.effects().emplace_back(effect_helper.toGenericDto());
    EffectTextureMapDtoHelper texture_helper;
    texture_helper.TextureMapping("earth.png", "APK_PATH", "earth", std::nullopt, "DiffuseMap");
    mesh_dto.textureMaps().emplace_back(texture_helper.toGenericDto());
    mesh_dto.renderListID() = Renderer::RenderListID::Scene;
    mesh_dto.visualTechniqueSelection() = "Default";

    Enigma::Frameworks::CommandBus::post(std::make_shared<Enigma::Engine::ConstitutePrimitive>(mesh_id, mesh_dto.toGenericDto()));
}

/*std::shared_ptr<MeshPrimitivePolicy> MeshPrimitiveMaker::MakeMeshPrimitivePolicy(const std::string& mesh_name, const std::string& geo_name)
{
    auto policy = std::make_shared<MeshPrimitivePolicy>();
    policy->Name() = mesh_name;
    policy->GeometryPolicy() = CubeGeometryMaker::MakeGeometryPolicy(geo_name);
    policy->EffectPolicies().emplace_back(MakeEffectPolicy("basic_vtx_tex"));
    policy->TexturePolicies().emplace_back(MakeTextureMapPolicy());
    return policy;
}

void MeshPrimitiveMaker::SaveMeshPrimitiveDto(const std::shared_ptr<MeshPrimitive>& mesh, const std::string& filename_at_path)
{
    mesh->GetGeometryData()->TheFactoryDesc().ClaimFromResource(mesh->GetGeometryData()->GetName(), "test_geometry.geo@DataPath");
    GenericDto dto = mesh->SerializeDto();
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto> {dto });
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(filename_at_path), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}

std::shared_ptr<MeshPrimitivePolicy> MeshPrimitiveMaker::LoadMeshPrimitivePolicy(const std::string& filename_at_path)
{
    IFilePtr readFile = FileSystem::Instance()->OpenFile(Filename(filename_at_path), "rb");
    size_t filesize = readFile->Size();
    auto read_buff = readFile->Read(0, filesize);
    std::string read_json = convert_to_string(read_buff.value(), read_buff->size());
    std::vector<GenericDto> read_dtos = DtoJsonGateway::Deserialize(read_json);
    assert(read_dtos.size() == 1);
    FileSystem::Instance()->CloseFile(readFile);
    if (read_dtos[0].GetRtti().GetRttiName() != MeshPrimitive::TYPE_RTTI.GetName()) return nullptr;
    return MeshPrimitiveDto::FromGenericDto(read_dtos[0]).ConvertToPolicy(std::make_shared<JsonFileDtoDeserializer>(),
        std::make_shared<JsonFileEffectProfileDeserializer>());
}

EffectMaterialPolicy MeshPrimitiveMaker::MakeEffectPolicy(const std::string& eff_name)
{
    return EffectMaterialPolicy{ eff_name, eff_name + ".efx@APK_PATH", std::make_shared<JsonFileEffectProfileDeserializer>() };
}

EffectTextureMapPolicy MeshPrimitiveMaker::MakeTextureMapPolicy()
{
    EffectTextureMapPolicy policy;
    policy.GetTuplePolicies().emplace_back(std::make_tuple( "DiffuseMap", TexturePolicy{"earth", "earth.png", "APK_PATH"}, std::nullopt));
    return policy;
}
*/
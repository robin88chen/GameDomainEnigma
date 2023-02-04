#include "SkinMeshModelMaker.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "Frameworks/StringFormat.h"
#include "Gateways/DtoJsonGateway.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "GameEngine/TriangleList.h"

using namespace Enigma::Renderer;
using namespace Enigma::MathLib;
using namespace Enigma::Gateways;
using namespace Enigma::Engine;

std::vector<std::string> bone_node_names{ "bone_root", "bone_one" };

std::shared_ptr<ModelPrimitivePolicy> SkinMeshModelMaker::MakeModelPrimitivePolicy(const std::string& model_name, const std::string& geo_name)
{
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
    /*auto animation = ModelAnimatorMaker::MakeModelAnimationAsset(model_name, node_names);
    auto animator = ModelAnimatorMaker::MakeModelAnimator(model_name, animation);
    dto.TheAnimator() = animator->SerializeDto().ToGenericDto();*/
    return dto.ConvertToPolicy(std::make_shared<JsonFileDtoDeserializer>(),
        std::make_shared<JsonFileEffectProfileDeserializer>());
}

SkinMeshPrimitiveDto SkinMeshModelMaker::MakeSkinMeshPrimitiveDto(const std::string& mesh_name, const std::string& geo_name)
{
    SkinMeshPrimitiveDto dto;
    dto.Name() = mesh_name;
    dto.GeometryName() = geo_name;
    dto.GeometryFactoryDesc() = FactoryDesc(TriangleList::TYPE_RTTI.GetName()).ClaimFromResource(geo_name, geo_name + ".geo", "DataPath");
    dto.Effects().emplace_back(MakeEffectDto("skinned_vtx_nor_tex").ToGenericDto());
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

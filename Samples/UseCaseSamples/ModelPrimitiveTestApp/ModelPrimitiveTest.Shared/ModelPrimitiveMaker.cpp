#include "ModelPrimitiveMaker.h"
#include "Renderer/RenderablePrimitivePolicies.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameEngine/TriangleList.h"
#include "Frameworks/StringFormat.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"

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

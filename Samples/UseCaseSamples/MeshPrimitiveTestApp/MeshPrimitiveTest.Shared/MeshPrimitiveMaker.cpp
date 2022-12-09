#include "MeshPrimitiveMaker.h"
#include "CubeGeometryMaker.h"
#include "Gateways/JsonFileEffectProfileDeserializer.h"
#include "GameEngine/TextureLoadingPolicies.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;

std::unique_ptr<MeshPrimitivePolicy> MeshPrimitiveMaker::MakeMeshPrimitivePolicy(const std::string& mesh_name, const std::string& geo_name)
{
    auto policy = std::make_unique<MeshPrimitivePolicy>();
    policy->Name() = mesh_name;
    policy->GeometryPolicy() = CubeGeometryMaker::MakeGeometryPolicy(geo_name);
    policy->EffectPolicies().emplace_back(MakeEffectPolicy("basic_vtx_tex"));
    policy->TexturePolicies().emplace_back(MakeTextureMapPolicy());
    return std::move(policy);
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

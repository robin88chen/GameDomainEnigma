#include "MeshPrimitiveMaker.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "CubeGeometryMaker.h"
#include "GameEngine/EffectDtoHelper.h"
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
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(mesh_id.name() + ".mesh@DataPath");
    mesh_dto.effects().emplace_back(EffectMaterialId("basic_vtx_tex"));
    EffectTextureMapDtoHelper texture_helper;
    texture_helper.TextureMapping(TextureId("earth"), std::nullopt, "DiffuseMap");
    mesh_dto.textureMaps().emplace_back(texture_helper.toGenericDto());
    mesh_dto.renderListID() = Renderer::RenderListID::Scene;
    mesh_dto.visualTechniqueSelection() = "Default";

    Enigma::Frameworks::CommandBus::post(std::make_shared<Enigma::Engine::ConstitutePrimitive>(mesh_id, mesh_dto.toGenericDto()));
}

#include "MeshPrimitiveMaker.h"
#include "CubeGeometryMaker.h"
#include "GameEngine/EffectDtoHelper.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "Renderables/MeshPrimitive.h"
#include "Renderables/RenderablePrimitiveDtos.h"
#include "Primitives/PrimitiveQueries.h"

using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;
using namespace Enigma::Geometries;
using namespace Enigma::Primitives;
using namespace Enigma::Renderer;

std::shared_ptr<MeshPrimitive> MeshPrimitiveMaker::makeCubeMeshPrimitive(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id)
{
    if (const auto mesh = Primitive::queryPrimitive(mesh_id))
    {
        return std::dynamic_pointer_cast<MeshPrimitive>(mesh);
    }
    MeshPrimitiveDto mesh_dto;
    mesh_dto.id() = mesh_id;
    mesh_dto.geometryId() = geo_id;
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(mesh_id.name() + ".mesh@DataPath");
    mesh_dto.effects().emplace_back(EffectMaterialId("basic_vtx_tex"));
    EffectTextureMapDtoHelper texture_helper;
    texture_helper.textureMapping(TextureId("earth"), std::nullopt, "DiffuseMap");
    mesh_dto.textureMaps().emplace_back(texture_helper.toGenericDto());
    mesh_dto.renderListID() = Renderer::RenderListID::Scene;
    mesh_dto.visualTechniqueSelection() = "Default";

    return std::dynamic_pointer_cast<MeshPrimitive>(std::make_shared<RequestPrimitiveConstitution>(mesh_id, mesh_dto.toGenericDto(), RequestPrimitiveConstitution::PersistenceLevel::Store)->dispatch());
}

#include "PrimitiveMeshMaker.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector2.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "GameEngine/BoundingVolume.h"
#include "GameEngine/EffectMaterial.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/Box3.h"
#include "MathLib/ContainmentBox3.h"
#include "Gateways/DtoJsonGateway.h"
#include "Geometries/GeometryDataDto.h"
#include "Geometries/TriangleList.h"
#include "Renderables/RenderablePrimitiveDtos.h"
#include "Renderables/MeshPrimitive.h"
#include "GameEngine/EffectDtoHelper.h"
#include <vector>

using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;
using namespace Enigma::FileSystem;
using namespace Enigma::Gateways;
using namespace Enigma::Renderer;
using namespace Enigma::Geometries;
using namespace Enigma::Primitives;
using namespace Enigma::Renderables;

BoundingVolume PrimitiveMeshMaker::m_floorBounding;
BoundingVolume PrimitiveMeshMaker::m_doorBounding;
BoundingVolume PrimitiveMeshMaker::m_boardBounding;

Enigma::Engine::GenericDto PrimitiveMeshMaker::makeFloorGeometry(const Enigma::Geometries::GeometryId& id)
{
    std::vector<Vector3> vtx_pos;
    vtx_pos.resize(121);
    std::vector<Vector3> vtx_nor;
    vtx_nor.resize(121);
    std::vector<Vector2> vtx_uv;
    vtx_uv.resize(121);
    for (unsigned int x = 0; x <= 10; x++)
    {
        for (unsigned int z = 0; z <= 10; z++)
        {
            vtx_pos[(z * 11) + x] = Vector3(((float)(x)-5.0f) * 2.0f, 0.0f, ((float)(z)-5.0f) * 2.0f);
            vtx_nor[(z * 11) + x] = Vector3(0.0f, 1.0f, 0.0f);
            vtx_uv[(z * 11) + x] = Vector2((float)x / 10.0f, 1.0f - (float)z / 10.0f);
        }
    }
    uint_buffer mesh_index;
    mesh_index.resize(600);
    for (unsigned int x = 0; x < 10; x++)
    {
        for (unsigned int z = 0; z < 10; z++)
        {
            unsigned int patch_idx = z * 10 + x;
            unsigned int vtx_idx = z * 11 + x;
            mesh_index[patch_idx * 6] = vtx_idx;
            mesh_index[patch_idx * 6 + 1] = vtx_idx + 11;
            mesh_index[patch_idx * 6 + 2] = vtx_idx + 12;
            mesh_index[patch_idx * 6 + 3] = vtx_idx;
            mesh_index[patch_idx * 6 + 4] = vtx_idx + 12;
            mesh_index[patch_idx * 6 + 5] = vtx_idx + 1;
        }
    }
    TextureCoordDto tex_dto;
    tex_dto.texture2DCoords() = vtx_uv;
    TriangleListDto dto;
    dto.position3s() = vtx_pos;
    dto.normals() = vtx_nor;
    dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    dto.indices() = mesh_index;
    dto.segments() = { 0, 121, 0, 600 };
    dto.vertexCapacity() = 121;
    dto.vertexUsedCount() = 121;
    dto.indexCapacity() = 600;
    dto.indexUsedCount() = 600;
    dto.id() = id;
    dto.vertexFormat() = "xyz_nor_tex1(2)";
    dto.topology() = static_cast<unsigned>(PrimitiveTopology::Topology_TriangleList);
    Box3 box = ContainmentBox3::ComputeAlignedBox(&vtx_pos[0], static_cast<unsigned>(vtx_pos.size()));
    m_floorBounding = BoundingVolume{ box };
    dto.geometryBound() = m_floorBounding.serializeDto().toGenericDto();
    dto.factoryDesc() = FactoryDesc(TriangleList::TYPE_RTTI.getName()).ClaimAsResourceAsset(id.name(), id.name() + ".geo", "DataPath");
    return dto.toGenericDto();
}

GenericDto PrimitiveMeshMaker::makeDoorGeometry(const GeometryId& id)
{
    std::vector<Vector3> pos =
    {
        Vector3(-1.5f, 0.0f, -1.5f), Vector3(-1.5f, 2.0f, -1.5f),
        Vector3(-0.5f, 2.0f, -1.5f), Vector3(-0.5f, 0.0f, -0.5f),
        Vector3(-1.5f, 3.0f, -1.5f), Vector3(-0.5f, 3.0f, -1.5f),
        Vector3(0.5f, 3.0f, -1.5f), Vector3(0.5f, 2.0f, -1.5f),
        Vector3(1.5f, 3.0f, -1.5f), Vector3(1.5f, 2.0f, -1.5f),
        Vector3(0.5f, 0.0f, -1.5f), Vector3(1.5f, 0.0f, -1.5f),
    };
    unsigned int vtx_count = pos.size();
    std::vector<Vector3> nor =
    {
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
    };
    std::vector<Vector2> uv =
    {
        Vector2(0.0f, 1.0f), Vector2(0.0f, 0.333f),
        Vector2(0.333f, 0.333f), Vector2(0.333f, 1.0f),
        Vector2(0.0f, 0.0f), Vector2(0.333f, 0.0f),
        Vector2(0.667f, 0.0f), Vector2(0.667f, 0.333f),
        Vector2(1.0f, 0.0f), Vector2(1.0f, 0.333f),
        Vector2(0.667f, 1.0f), Vector2(1.0f, 1.0f),
    };
    uint_buffer idx =
    {
        0,1,2, 0,2,3,
        1,4,5, 1,5,2,
        2,5,6, 2,6,7,
        7,6,8, 7,8,9,
        10,7,9, 10,9,11,

        0,2,1, 0,3,2,
        1,5,4, 1,2,5,
        2,6,5, 2,7,6,
        7,8,6, 7,9,8,
        10,9,7, 10,11,9,
    };
    TextureCoordDto tex_dto;
    tex_dto.texture2DCoords() = uv;
    TriangleListDto dto;
    dto.position3s() = pos;
    dto.normals() = nor;
    dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    dto.indices() = idx;
    dto.segments() = { 0, static_cast<unsigned>(pos.size()), 0, static_cast<unsigned>(idx.size()) };
    dto.vertexCapacity() = pos.size();
    dto.vertexUsedCount() = pos.size();
    dto.indexCapacity() = idx.size();
    dto.indexUsedCount() = idx.size();
    dto.id() = id;
    dto.vertexFormat() = "xyz_nor_tex1(2)";
    dto.topology() = static_cast<unsigned>(PrimitiveTopology::Topology_TriangleList);
    Box3 box = ContainmentBox3::ComputeAlignedBox(&pos[0], static_cast<unsigned>(pos.size()));
    m_doorBounding = BoundingVolume{ box };
    dto.geometryBound() = m_doorBounding.serializeDto().toGenericDto();
    dto.factoryDesc() = FactoryDesc(TriangleList::TYPE_RTTI.getName()).ClaimAsResourceAsset(id.name(), id.name() + ".geo", "DataPath");
    return dto.toGenericDto();
}

GenericDto PrimitiveMeshMaker::makeBoardGeometry(const GeometryId& id)
{
    std::vector<Vector3> pos =
    {
        Vector3(-0.5f, 0.0f, 0.0f), Vector3(-0.5f, 4.0f, 0.0f),
        Vector3(0.5f, 4.0f, 0.0f), Vector3(0.5f, 0.0f, 0.0f),
    };
    unsigned int vtx_count = pos.size();
    std::vector<Vector3> nor =
    {
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
        Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 0.0f, -1.0f),
    };
    std::vector<Vector2> uv =
    {
        Vector2(0.0f, 1.0f), Vector2(0.0f, 0.0f),
        Vector2(1.0f, 0.0f), Vector2(1.0f, 1.0f),
    };
    uint_buffer idx =
    {
        0,1,2, 0,2,3,
    };
    TextureCoordDto tex_dto;
    tex_dto.texture2DCoords() = uv;
    TriangleListDto dto;
    dto.position3s() = pos;
    dto.normals() = nor;
    dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    dto.indices() = idx;
    dto.segments() = { 0, static_cast<unsigned>(pos.size()), 0, static_cast<unsigned>(idx.size()) };
    dto.vertexCapacity() = pos.size();
    dto.vertexUsedCount() = pos.size();
    dto.indexCapacity() = idx.size();
    dto.indexUsedCount() = idx.size();
    dto.id() = id;
    dto.vertexFormat() = "xyz_nor_tex1(2)";
    dto.topology() = static_cast<unsigned>(PrimitiveTopology::Topology_TriangleList);
    Box3 box = ContainmentBox3::ComputeAlignedBox(&pos[0], static_cast<unsigned>(pos.size()));
    m_boardBounding = BoundingVolume{ box };
    dto.geometryBound() = m_boardBounding.serializeDto().toGenericDto();
    dto.factoryDesc() = FactoryDesc(TriangleList::TYPE_RTTI.getName()).ClaimAsResourceAsset(id.name(), id.name() + ".geo", "DataPath");
    return dto.toGenericDto();
}

GenericDto PrimitiveMeshMaker::makeMeshPrimitive(const PrimitiveId& mesh_id, const GeometryId& geo_id,
    const EffectMaterialId& effect_id, const TextureId& texture_id, const std::string& tex_semantic)
{
    MeshPrimitiveDto mesh_dto;
    mesh_dto.id() = mesh_id;
    mesh_dto.geometryId() = geo_id;
    mesh_dto.factoryDesc() = FactoryDesc(MeshPrimitive::TYPE_RTTI.getName()).ClaimAsNative(mesh_id.name() + ".mesh@DataPath");
    mesh_dto.effects().emplace_back(effect_id);
    EffectTextureMapDtoHelper texture_helper;
    texture_helper.textureMapping(texture_id, std::nullopt, tex_semantic);
    mesh_dto.textureMaps().emplace_back(texture_helper.toGenericDto());
    mesh_dto.renderListID() = Renderer::RenderListID::Scene;
    mesh_dto.visualTechniqueSelection() = "Default";
    return mesh_dto.toGenericDto();
}

#include "PrimitiveMeshMaker.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector2.h"
#include "Frameworks/ExtentTypesDefine.h"
#include <vector>
#include <FileSystem/Filename.h>
#include <FileSystem/IFile.h>
#include "FileSystem/FileSystem.h"
#include <GameEngine/BoundingVolume.h>
#include <GameEngine/EffectMaterial.h>
#include <GameEngine/GeometryDataDto.h>
#include <GraphicKernel/GraphicAPITypes.h>
#include <MathLib/Box3.h>
#include <Platforms/PlatformLayer.h>
#include "MathLib/ContainmentBox3.h"
#include "GameEngine/TriangleList.h"
#include "Gateways/DtoJsonGateway.h"
#include "Gateways/JsonFileDtoDeserializer.h"

using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;
using namespace Enigma::FileSystem;
using namespace Enigma::Gateways;
using namespace Enigma::Renderer;

BoundingVolume PrimitiveMeshMaker::m_floorBounding;

void PrimitiveMeshMaker::MakeSavedFloorGeometry(const std::string& name)
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
    tex_dto.Texture2DCoords() = vtx_uv;
    TriangleListDto dto;
    dto.Position3s() = vtx_pos;
    dto.Normals() = vtx_nor;
    dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    dto.Indices() = mesh_index;
    dto.Segments() = { 0, 121, 0, 600 };
    dto.VertexCapacity() = 121;
    dto.VertexUsedCount() = 121;
    dto.IndexCapacity() = 600;
    dto.IndexUsedCount() = 600;
    dto.Name() = name;
    dto.VertexFormat() = "xyz_nor_tex1(2)";
    dto.Topology() = static_cast<unsigned>(PrimitiveTopology::Topology_TriangleList);
    Box3 box = ContainmentBox3::ComputeAlignedBox(&vtx_pos[0], static_cast<unsigned>(vtx_pos.size()));
    m_floorBounding = BoundingVolume{ box };
    dto.GeometryBound() = m_floorBounding.SerializeDto().ToGenericDto();

    GenericDto gen_dto = dto.ToGenericDto();
    FactoryDesc desc(TriangleList::TYPE_RTTI.GetName());
    desc.ClaimAsResourceAsset(name, name + ".geo", "DataPath");
    gen_dto.AddRtti(desc);
    std::string json = DtoJsonGateway::Serialize(std::vector<GenericDto>{gen_dto});

    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(name + ".geo@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}

Enigma::Renderer::MeshPrimitiveDto PrimitiveMeshMaker::MakeMeshPrimitiveDto(const std::string& mesh_name, const std::string& geo_name,
    const std::string& eff_name, const std::string& eff_filename,
    const std::string& tex_filename, const std::string& tex_name, const std::string& tex_semantic)
{
    MeshPrimitiveDto dto;
    dto.Name() = mesh_name;
    dto.GeometryName() = geo_name;
    dto.GeometryFactoryDesc() = FactoryDesc(TriangleList::TYPE_RTTI.GetName()).ClaimFromResource(geo_name, geo_name + ".geo", "DataPath");
    dto.Effects().emplace_back(MakeEffectDto(eff_name, eff_filename).ToGenericDto());
    dto.TextureMaps().emplace_back(MakeTextureMapDto(tex_filename, tex_name, tex_semantic).ToGenericDto());
    return dto;
}

EffectMaterialDto PrimitiveMeshMaker::MakeEffectDto(const std::string& eff_name, const std::string& eff_filename)
{
    EffectMaterialDto dto;
    dto.Name() = eff_name;
    dto.TheFactoryDesc() = FactoryDesc(EffectMaterial::TYPE_RTTI.GetName()).ClaimFromResource(eff_name, eff_filename, "APK_PATH");
    return dto;
}

EffectTextureMapDto PrimitiveMeshMaker::MakeTextureMapDto(const std::string& filename, const std::string& tex_name, const std::string& semantic)
{
    EffectTextureMapDto dto;
    TextureMappingDto tex;
    tex.Filename() = filename;
    tex.Semantic() = semantic;
    tex.TextureName() = tex_name;
    tex.PathId() = "APK_PATH";
    dto.TextureMappings().emplace_back(tex);
    return dto;
}

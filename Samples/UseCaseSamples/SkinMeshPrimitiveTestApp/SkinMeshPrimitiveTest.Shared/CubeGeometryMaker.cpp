#include "CubeGeometryMaker.h"
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "GameEngine/GeometryDataPolicy.h"
#include "GameEngine/GeometryDataDto.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/GeometryRepository.h"
#include "MathLib/ContainmentBox3.h"
#include "GameEngine/BoundingVolume.h"
#include "GameEngine/BoundingVolumeDto.h"
#include "FileSystem/FileSystem.h"
#include "Platforms/PlatformLayer.h"
#include "GameEngine/TriangleList.h"
#include "Gateways/DtoJsonGateway.h"
#include "Gateways/JsonFileDtoDeserializer.h"

using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Graphics;
using namespace Enigma::FileSystem;
using namespace Enigma::Gateways;

void CubeGeometryMaker::MakeSavedCube(const std::string& name)
{
    std::vector<Vector3> vtx_pos =
    {
        { Vector3(-0.5f, -0.5f, -0.5f) },
        { Vector3(0.5f, -0.5f, -0.5f) },
        { Vector3(0.5f, -0.5f,  0.5f) },
        { Vector3(-0.5f, -0.5f,  0.5f) },

        { Vector3(-0.5f,  0.5f, -0.5f) },
        { Vector3(0.5f,  0.5f, -0.5f) },
        { Vector3(0.5f,  0.5f,  0.5f) },
        { Vector3(-0.5f,  0.5f,  0.5f) },

        { Vector3(-0.5f,  1.5f, -0.5f) },
        { Vector3(0.5f,  1.5f, -0.5f) },
        { Vector3(0.5f,  1.5f,  0.5f) },
        { Vector3(-0.5f,  1.5f,  0.5f) },

        { Vector3(-0.5f,  2.5f, -0.5f) },
        { Vector3(0.5f,  2.5f, -0.5f) },
        { Vector3(0.5f,  2.5f,  0.5f) },
        { Vector3(-0.5f,  2.5f,  0.5f) },
    };
    std::vector<Vector3> vtx_nor =
    {
        { Vector3(-1, -1, -1) },
        { Vector3(1, -1, -1) },
        { Vector3(1, -1,  1) },
        { Vector3(-1, -1,  1) },

        { Vector3(-1, 0, -1) },
        { Vector3(1, 0, -1) },
        { Vector3(1, 0,  1) },
        { Vector3(-1, 0,  1) },

        { Vector3(-1, 0, -1) },
        { Vector3(1, 0, -1) },
        { Vector3(1, 0,  1) },
        { Vector3(-1, 0,  1) },

        { Vector3(-1,  1, -1) },
        { Vector3(1,  1, -1) },
        { Vector3(1,  1,  1) },
        { Vector3(-1,  1,  1) },
    };
    std::vector<Vector2> vtx_uv =
    {
        { Vector2(0.0f, 0.0f) },
        { Vector2(1.0f, 0.0f) },
        { Vector2(0.0f, 0.0f) },
        { Vector2(1.0f, 0.0f) },

        { Vector2(0.0f, 1.0f) },
        { Vector2(1.0f, 1.0f) },
        { Vector2(0.0f, 1.0f) },
        { Vector2(1.0f, 1.0f) },

        { Vector2(0.0f, 2.0f) },
        { Vector2(1.0f, 2.0f) },
        { Vector2(0.0f, 2.0f) },
        { Vector2(1.0f, 2.0f) },

        { Vector2(0.0f, 3.0f) },
        { Vector2(1.0f, 3.0f) },
        { Vector2(0.0f, 3.0f) },
        { Vector2(1.0f, 3.0f) },
    };
    uint_buffer vtx_idx =
    {
        0, 1, 2, 0, 2, 3,

        0, 5, 1, 0, 4, 5,
        1, 6, 2, 1, 5, 6,
        2, 7, 3, 2, 6, 7,
        3, 4, 0, 3, 7, 4,

        4, 9, 5, 4, 8, 9,
        5, 10, 6, 5, 9, 10,
        6, 11, 7, 6, 10, 11,
        7, 8, 4, 7, 11, 8,

        8, 13, 9, 8, 12, 13,
        9, 14, 10, 9, 13, 14,
        10, 15, 11, 10, 14, 15,
        11, 12, 8, 11, 15, 12,

        12, 14, 13, 12, 15, 14,
    };
    std::vector<unsigned> palette =
    {
        256,256,256,256,
        256,256,256,256,
        256,256,256,256,
        256,256,256,256,
    };
    std::vector<float> weight0 =
    {
        1.0f,1.0f,1.0f,1.0f,
        1.0f,1.0f,1.0f,1.0f,
        0.0f,0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,0.0f,
    };
    TextureCoordDto tex_dto;
    tex_dto.Texture2DCoords() = vtx_uv;
    TriangleListDto dto;
    dto.Position3s() = vtx_pos;
    for (auto& n : vtx_nor)
    {
        n.NormalizeSelf();
    }
    dto.Normals() = vtx_nor;
    dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    dto.Indices() = vtx_idx;
    dto.PaletteIndices() = palette;
    dto.Weights() = weight0;
    dto.Segments() = { 0, 16, 0, 84 };
    dto.VertexCapacity() = 16;
    dto.VertexUsedCount() = 16;
    dto.IndexCapacity() = 84;
    dto.IndexUsedCount() = 84;
    dto.Name() = name;
    dto.VertexFormat() = "xyzb2_nor_tex1(2)_betabyte";
    dto.Topology() = static_cast<unsigned>(PrimitiveTopology::Topology_TriangleList);
    Box3 box = ContainmentBox3::ComputeAlignedBox(&vtx_pos[0], static_cast<unsigned>(vtx_pos.size()));
    BoundingVolume bv{ box };
    dto.GeometryBound() = bv.SerializeDto().ToGenericDto();

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

GeometryDataPolicy CubeGeometryMaker::MakeGeometryPolicy(const std::string& name)
{
    return GeometryDataPolicy{ name, name + ".geo@DataPath", std::make_shared<JsonFileDtoDeserializer>() };
}

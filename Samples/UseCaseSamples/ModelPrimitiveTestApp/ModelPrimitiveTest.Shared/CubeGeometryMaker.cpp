#include "CubeGeometryMaker.h"
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "MathLib/ContainmentBox3.h"
#include "GameEngine/BoundingVolume.h"
#include "GameEngine/BoundingVolumeDto.h"
#include "FileSystem/FileSystem.h"
#include "Platforms/PlatformLayer.h"
#include "Gateways/DtoJsonGateway.h"
#include "Gateways/JsonFileDtoDeserializer.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "Geometries/GeometryDataDto.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "Geometries/TriangleList.h"
#include "Frameworks/CommandBus.h"
#include "Geometries/GeometryCommands.h"

using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::FileSystem;
using namespace Enigma::Gateways;
using namespace Enigma::Geometries;

void CubeGeometryMaker::makeCube(const GeometryId& id)
{
    struct VtxData
    {
        Vector3 pos;
        Vector2 coord;
    };
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
    };
    std::vector<Vector2> vtx_uv =
    {
        { Vector2(0.0f, 0.0f)},
        { Vector2(1.0f, 0.0f) },
        { Vector2(0.0f, 0.0f) },
        { Vector2(1.0f, 0.0f) },
        { Vector2(0.0f, 1.0f)},
        { Vector2(1.0f, 1.0f) },
        { Vector2(0.0f, 1.0f) },
        { Vector2(1.0f, 1.0f) },
    };
    uint_buffer vtx_idx =
    {
        0, 5, 1, 0, 4, 5,
        1, 6, 2, 1, 5, 6,
        2, 7, 3, 2, 6, 7,
        3, 4, 0, 3, 7, 4,
        4, 6, 5, 4, 7, 6,
        0, 1, 2, 0, 2, 3,
    };
    TextureCoordDto tex_dto;
    tex_dto.texture2DCoords() = vtx_uv;
    TriangleListDto dto;
    dto.position3s() = vtx_pos;
    dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    dto.indices() = vtx_idx;
    dto.segments() = { 0, 8, 0, 36 };
    dto.vertexCapacity() = 8;
    dto.vertexUsedCount() = 8;
    dto.indexCapacity() = 36;
    dto.indexUsedCount() = 36;
    dto.id() = id;
    dto.vertexFormat() = "xyz_tex1(2)";
    dto.topology() = static_cast<unsigned>(Enigma::Graphics::PrimitiveTopology::Topology_TriangleList);
    Box3 box = ContainmentBox3::ComputeAlignedBox(&vtx_pos[0], vtx_pos.size());
    BoundingVolume bv{ box };
    dto.geometryBound() = bv.serializeDto().toGenericDto();
    dto.factoryDesc() = FactoryDesc(TriangleList::TYPE_RTTI.getName()).ClaimAsResourceAsset(id.name(), id.name() + ".geo", "DataPath");

    Enigma::Frameworks::CommandBus::post(std::make_shared<ConstituteGeometry>(id, dto.toGenericDto()));
}

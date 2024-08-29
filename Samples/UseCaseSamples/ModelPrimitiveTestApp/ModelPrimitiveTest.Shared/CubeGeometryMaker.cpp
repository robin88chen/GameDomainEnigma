#include "CubeGeometryMaker.h"

#include <GameEngine/EffectTextureMapAssembler.h>

#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "MathLib/ContainmentBox3.h"
#include "GameEngine/BoundingVolume.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "Geometries/TriangleListAssembler.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "Geometries/TriangleList.h"
#include "Geometries/GeometryDataQueries.h"

using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Geometries;

std::shared_ptr<GeometryData> CubeGeometryMaker::makeCube(const GeometryId& id)
{
    if (const auto geo = GeometryData::queryGeometryData(id)) return geo;

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
    TriangleListAssembler assembler(id);
    assembler.position3s(vtx_pos);
    assembler.addTexture2DCoords(vtx_uv);
    assembler.indices(vtx_idx);
    assembler.addSegment({ 0, 8, 0, 36 });
    assembler.vertexCapacity(8);
    assembler.vertexUsedCount(8);
    assembler.indexCapacity(36);
    assembler.indexUsedCount(36);
    assembler.topology(Enigma::Graphics::PrimitiveTopology::Topology_TriangleList);
    assembler.asAsset(id.name(), id.name() + ".geo", "DataPath");
    assembler.computeAlignedBox();

    return std::make_shared<RequestGeometryConstitution>(id, assembler.assemble())->dispatch();
}

#include "CubeGeometryMaker.h"
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include "MathLib/ContainmentBox3.h"
#include "GameEngine/BoundingVolume.h"
#include "Geometries/TriangleList.h"
#include "Geometries/GeometryDataQueries.h"
#include "Geometries/GeometryCommands.h"
#include "Geometries/TriangleListAssembler.h"

using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Geometries;

BoundingVolume CubeGeometryMaker::m_geometryBounding;

std::shared_ptr<GeometryData> CubeGeometryMaker::makeCube(const GeometryId& id)
{
    if (auto geo = GeometryData::queryGeometryData(id)) return geo;

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
    TriangleListAssembler assembler(id);
    assembler.addTexture2DCoords(vtx_uv);
    assembler.position3s(vtx_pos);
    for (auto& n : vtx_nor)
    {
        n.normalizeSelf();
    }
    assembler.normals(vtx_nor);
    assembler.indices(vtx_idx);
    assembler.paletteIndices(palette);
    assembler.weights(weight0, 1);
    assembler.addSegment({ 0, 16, 0, 84 });
    assembler.vertexCapacity(16);
    assembler.vertexUsedCount(16);
    assembler.indexCapacity(84);
    assembler.indexUsedCount(84);
    assembler.topology(Enigma::Graphics::PrimitiveTopology::Topology_TriangleList);
    assembler.asAsset(id.name(), id.name() + ".geo", "DataPath");
    assembler.computeAlignedBox();
    m_geometryBounding = assembler.geometryBound();

    auto geometry = std::make_shared<RequestGeometryConstitution>(id, assembler.assemble())->dispatch();
    std::make_shared<PutGeometry>(id, geometry)->execute();
    return geometry;
}

#include "StandardGeometryDtoHelper.h"
#include "GameEngine/BoundingVolume.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/ContainmentBox3.h"
#include "MathLib/MathGlobal.h"

using namespace Enigma::Geometries;
using namespace Enigma::Engine;

SquareQuadDtoHelper::SquareQuadDtoHelper(const GeometryId& id)
{
    m_dto.id() = id;
}

SquareQuadDtoHelper& SquareQuadDtoHelper::xyQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top)
{
    std::vector<MathLib::Vector3> positions;
    positions.emplace_back(left_bottom);
    positions.emplace_back(MathLib::Vector3(left_bottom.x(), right_top.y(), left_bottom.z()));
    positions.emplace_back(MathLib::Vector3(right_top.x(), right_top.y(), left_bottom.z()));
    positions.emplace_back(MathLib::Vector3(right_top.x(), left_bottom.y(), left_bottom.z()));
    m_dto.position3s() = positions;
    m_normal = MathLib::Vector3(0, 0, 1);
    uint_buffer indices =
    {
        0,1,2, 0,2,3
    };
    m_dto.indices() = indices;
    m_dto.segments() = { 0, 4, 0, 6 };
    m_dto.vertexUsedCount() = 4;
    m_dto.indexUsedCount() = 6;
    m_dto.vertexCapacity() = 4;
    m_dto.indexCapacity() = 6;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;
    return *this;
}

SquareQuadDtoHelper& SquareQuadDtoHelper::xzQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top)
{
    std::vector<MathLib::Vector3> positions;
    positions.emplace_back(left_bottom);
    positions.emplace_back(MathLib::Vector3(left_bottom.x(), left_bottom.y(), right_top.z()));
    positions.emplace_back(MathLib::Vector3(right_top.x(), left_bottom.y(), right_top.z()));
    positions.emplace_back(MathLib::Vector3(right_top.x(), left_bottom.y(), left_bottom.z()));
    m_dto.position3s() = positions;
    m_normal = MathLib::Vector3(0, 1, 0);
    uint_buffer indices =
    {
        0,1,2, 0,2,3
    };
    m_dto.indices() = indices;
    m_dto.segments() = { 0, 4, 0, 6 };
    m_dto.vertexUsedCount() = 4;
    m_dto.indexUsedCount() = 6;
    m_dto.vertexCapacity() = 4;
    m_dto.indexCapacity() = 6;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;
    return *this;

}

SquareQuadDtoHelper& SquareQuadDtoHelper::normal()
{
    std::vector<MathLib::Vector3> normals;
    normals.emplace_back(m_normal);
    normals.emplace_back(m_normal);
    normals.emplace_back(m_normal);
    normals.emplace_back(m_normal);
    m_dto.normals() = normals;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    return *this;
}

SquareQuadDtoHelper& SquareQuadDtoHelper::textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    std::vector<MathLib::Vector2> texcoords;
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), right_top.y()));
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    size_t tex_stage_count = m_dto.textureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.texture2DCoords() = texcoords;
    m_dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 2;
    m_format.m_texCount = static_cast<unsigned>(m_dto.textureCoords().size());
    return *this;
}

SquareQuadDtoHelper& SquareQuadDtoHelper::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_dto.factoryDesc().ClaimAsResourceAsset(name, filename, path_id);
    return *this;
}

GenericDto SquareQuadDtoHelper::toGenericDto()
{
    assert(m_dto.position3s() || m_dto.position4s());
    m_dto.vertexFormat() = m_format.ToString();
    m_dto.topology() = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    if (auto& pos3 = m_dto.position3s())
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos3.value()[0]), static_cast<unsigned>(pos3.value().size()));
        const auto bb = BoundingVolume{ box };
        m_dto.geometryBound() = bb.serializeDto().toGenericDto();
    }
    else if (auto& pos4 = m_dto.position4s())
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos4.value()[0]), static_cast<unsigned>(pos4.value().size()));
        const auto bb = BoundingVolume{ box };
        m_dto.geometryBound() = bb.serializeDto().toGenericDto();
    }
    return m_dto.toGenericDto();
}

CubeDtoHelper::CubeDtoHelper(const GeometryId& id)
{
    m_dto.id() = id;
}

CubeDtoHelper& CubeDtoHelper::cube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent)
{
    std::vector<MathLib::Vector3> positions;
    MathLib::Vector3 xyz0 = center - axis_extent;
    MathLib::Vector3 xyz1 = center + axis_extent;
    positions.emplace_back(xyz0);
    positions.emplace_back(MathLib::Vector3(xyz1.x(), xyz0.y(), xyz0.z()));
    positions.emplace_back(MathLib::Vector3(xyz1.x(), xyz0.y(), xyz1.z()));
    positions.emplace_back(MathLib::Vector3(xyz0.x(), xyz0.y(), xyz1.z()));
    positions.emplace_back(MathLib::Vector3(xyz0.x(), xyz1.y(), xyz0.z()));
    positions.emplace_back(MathLib::Vector3(xyz1.x(), xyz1.y(), xyz0.z()));
    positions.emplace_back(xyz1);
    positions.emplace_back(MathLib::Vector3(xyz0.x(), xyz1.y(), xyz1.z()));
    m_dto.position3s() = positions;
    uint_buffer indices =
    {
        0, 1, 2, 0, 2, 3,

        0, 5, 1, 0, 4, 5,
        1, 6, 2, 1, 5, 6,
        2, 7, 3, 2, 6, 7,
        3, 4, 0, 3, 7, 4,

        4, 6, 5, 4, 7, 6
    };
    m_dto.indices() = indices;
    m_dto.segments() = { 0, 8, 0, 36 };
    m_dto.vertexUsedCount() = 8;
    m_dto.indexUsedCount() = 36;
    m_dto.vertexCapacity() = 8;
    m_dto.indexCapacity() = 36;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;
    return *this;
}

CubeDtoHelper& CubeDtoHelper::facedCube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent)
{
    std::vector<MathLib::Vector3> positions;
    MathLib::Vector3 xyz0 = center - axis_extent;
    MathLib::Vector3 xyz6 = center + axis_extent;
    MathLib::Vector3 xyz1(xyz6.x(), xyz0.y(), xyz0.z());
    MathLib::Vector3 xyz2(xyz6.x(), xyz0.y(), xyz6.z());
    MathLib::Vector3 xyz3(xyz0.x(), xyz0.y(), xyz6.z());
    MathLib::Vector3 xyz4(xyz0.x(), xyz6.y(), xyz0.z());
    MathLib::Vector3 xyz5(xyz6.x(), xyz6.y(), xyz0.z());
    MathLib::Vector3 xyz7(xyz0.x(), xyz6.y(), xyz6.z());
    positions.emplace_back(xyz0);
    positions.emplace_back(xyz1);
    positions.emplace_back(xyz2);
    positions.emplace_back(xyz3);
    positions.emplace_back(xyz0);
    positions.emplace_back(xyz4);
    positions.emplace_back(xyz5);
    positions.emplace_back(xyz1);
    positions.emplace_back(xyz1);
    positions.emplace_back(xyz5);
    positions.emplace_back(xyz6);
    positions.emplace_back(xyz2);
    positions.emplace_back(xyz2);
    positions.emplace_back(xyz6);
    positions.emplace_back(xyz7);
    positions.emplace_back(xyz3);
    positions.emplace_back(xyz3);
    positions.emplace_back(xyz7);
    positions.emplace_back(xyz4);
    positions.emplace_back(xyz0);
    positions.emplace_back(xyz4);
    positions.emplace_back(xyz7);
    positions.emplace_back(xyz6);
    positions.emplace_back(xyz5);
    m_dto.position3s() = positions;
    uint_buffer indices =
    {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23,
    };
    m_dto.indices() = indices;
    m_dto.segments() = { 0, 24, 0, 36 };
    m_dto.vertexUsedCount() = 24;
    m_dto.indexUsedCount() = 36;
    m_dto.vertexCapacity() = 24;
    m_dto.indexCapacity() = 36;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;
    return *this;
}

CubeDtoHelper& CubeDtoHelper::normal()
{
    std::vector<MathLib::Vector3> normals;
    normals.emplace_back(MathLib::Vector3(-1, -1, -1).normalize());
    normals.emplace_back(MathLib::Vector3(1, -1, -1).normalize());
    normals.emplace_back(MathLib::Vector3(1, -1, 1).normalize());
    normals.emplace_back(MathLib::Vector3(-1, -1, 1).normalize());
    normals.emplace_back(MathLib::Vector3(-1, 1, -1).normalize());
    normals.emplace_back(MathLib::Vector3(1, 1, -1).normalize());
    normals.emplace_back(MathLib::Vector3(1, 1, 1).normalize());
    normals.emplace_back(MathLib::Vector3(-1, 1, 1).normalize());
    m_dto.normals() = normals;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    return *this;
}

CubeDtoHelper& CubeDtoHelper::facedNormal()
{
    std::vector<MathLib::Vector3> normals;
    normals.emplace_back(-MathLib::Vector3::UNIT_Y);
    normals.emplace_back(-MathLib::Vector3::UNIT_Y);
    normals.emplace_back(-MathLib::Vector3::UNIT_Y);
    normals.emplace_back(-MathLib::Vector3::UNIT_Y);
    normals.emplace_back(-MathLib::Vector3::UNIT_Z);
    normals.emplace_back(-MathLib::Vector3::UNIT_Z);
    normals.emplace_back(-MathLib::Vector3::UNIT_Z);
    normals.emplace_back(-MathLib::Vector3::UNIT_Z);
    normals.emplace_back(MathLib::Vector3::UNIT_X);
    normals.emplace_back(MathLib::Vector3::UNIT_X);
    normals.emplace_back(MathLib::Vector3::UNIT_X);
    normals.emplace_back(MathLib::Vector3::UNIT_X);
    normals.emplace_back(MathLib::Vector3::UNIT_Z);
    normals.emplace_back(MathLib::Vector3::UNIT_Z);
    normals.emplace_back(MathLib::Vector3::UNIT_Z);
    normals.emplace_back(MathLib::Vector3::UNIT_Z);
    normals.emplace_back(-MathLib::Vector3::UNIT_X);
    normals.emplace_back(-MathLib::Vector3::UNIT_X);
    normals.emplace_back(-MathLib::Vector3::UNIT_X);
    normals.emplace_back(-MathLib::Vector3::UNIT_X);
    normals.emplace_back(MathLib::Vector3::UNIT_Y);
    normals.emplace_back(MathLib::Vector3::UNIT_Y);
    normals.emplace_back(MathLib::Vector3::UNIT_Y);
    normals.emplace_back(MathLib::Vector3::UNIT_Y);
    m_dto.normals() = normals;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    return *this;
}

CubeDtoHelper& CubeDtoHelper::textureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    std::vector<MathLib::Vector2> texcoords;
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), right_top.y()));
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), right_top.y()));
    size_t tex_stage_count = m_dto.textureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.texture2DCoords() = texcoords;
    m_dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 2;
    m_format.m_texCount = static_cast<unsigned>(m_dto.textureCoords().size());
    return *this;
}

CubeDtoHelper& CubeDtoHelper::facedTextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    std::vector<MathLib::Vector2> texcoords;
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.x(), right_top.y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.x(), left_bottom.y()));
    size_t tex_stage_count = m_dto.textureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.texture2DCoords() = texcoords;
    m_dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 2;
    m_format.m_texCount = static_cast<unsigned>(m_dto.textureCoords().size());
    return *this;
}

CubeDtoHelper& CubeDtoHelper::textureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back)
{
    std::vector<MathLib::Vector3> texcoords;
    texcoords.emplace_back(left_bottom_front);
    texcoords.emplace_back(MathLib::Vector3(right_top_back.x(), left_bottom_front.y(), left_bottom_front.z()));
    texcoords.emplace_back(MathLib::Vector3(right_top_back.x(), left_bottom_front.y(), right_top_back.z()));
    texcoords.emplace_back(MathLib::Vector3(left_bottom_front.x(), left_bottom_front.y(), right_top_back.z()));
    texcoords.emplace_back(MathLib::Vector3(left_bottom_front.x(), right_top_back.y(), left_bottom_front.z()));
    texcoords.emplace_back(MathLib::Vector3(right_top_back.x(), right_top_back.y(), left_bottom_front.z()));
    texcoords.emplace_back(MathLib::Vector3(right_top_back));
    texcoords.emplace_back(MathLib::Vector3(left_bottom_front.x(), right_top_back.y(), right_top_back.z()));
    size_t tex_stage_count = m_dto.textureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.texture3DCoords() = texcoords;
    m_dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 3;
    m_format.m_texCount = static_cast<unsigned>(m_dto.textureCoords().size());
    return *this;
}

CubeDtoHelper& CubeDtoHelper::facedTextureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back)
{
    std::vector<MathLib::Vector3> texcoords;
    MathLib::Vector3 xyz0 = left_bottom_front;
    MathLib::Vector3 xyz6 = right_top_back;
    MathLib::Vector3 xyz1(xyz6.x(), xyz0.y(), xyz0.z());
    MathLib::Vector3 xyz2(xyz6.x(), xyz0.y(), xyz6.z());
    MathLib::Vector3 xyz3(xyz0.x(), xyz0.y(), xyz6.z());
    MathLib::Vector3 xyz4(xyz0.x(), xyz6.y(), xyz0.z());
    MathLib::Vector3 xyz5(xyz6.x(), xyz6.y(), xyz0.z());
    MathLib::Vector3 xyz7(xyz0.x(), xyz6.y(), xyz6.z());
    texcoords.emplace_back(xyz0);
    texcoords.emplace_back(xyz1);
    texcoords.emplace_back(xyz2);
    texcoords.emplace_back(xyz3);
    texcoords.emplace_back(xyz0);
    texcoords.emplace_back(xyz4);
    texcoords.emplace_back(xyz5);
    texcoords.emplace_back(xyz1);
    texcoords.emplace_back(xyz1);
    texcoords.emplace_back(xyz5);
    texcoords.emplace_back(xyz6);
    texcoords.emplace_back(xyz2);
    texcoords.emplace_back(xyz2);
    texcoords.emplace_back(xyz6);
    texcoords.emplace_back(xyz7);
    texcoords.emplace_back(xyz3);
    texcoords.emplace_back(xyz3);
    texcoords.emplace_back(xyz7);
    texcoords.emplace_back(xyz4);
    texcoords.emplace_back(xyz0);
    texcoords.emplace_back(xyz4);
    texcoords.emplace_back(xyz7);
    texcoords.emplace_back(xyz6);
    texcoords.emplace_back(xyz5);
    size_t tex_stage_count = m_dto.textureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.texture3DCoords() = texcoords;
    m_dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 3;
    m_format.m_texCount = static_cast<unsigned>(m_dto.textureCoords().size());
    return *this;
}

CubeDtoHelper& CubeDtoHelper::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_dto.factoryDesc().ClaimAsResourceAsset(name, filename, path_id);
    return *this;
}

GenericDto CubeDtoHelper::toGenericDto()
{
    assert(m_dto.position3s() || m_dto.position4s());
    m_dto.vertexFormat() = m_format.ToString();
    m_dto.topology() = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    if (auto& pos = m_dto.position3s())
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos.value()[0]), static_cast<unsigned>(pos.value().size()));
        const auto bb = BoundingVolume{ box };
        m_dto.geometryBound() = bb.serializeDto().toGenericDto();
    }
    else if (auto& pos4 = m_dto.position4s())
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos4.value()[0]), static_cast<unsigned>(pos4.value().size()));
        const auto bb = BoundingVolume{ box };
        m_dto.geometryBound() = bb.serializeDto().toGenericDto();
    }
    return m_dto.toGenericDto();
}

SphereDtoHelper::SphereDtoHelper(const GeometryId& id)
{
    m_radius = 1.0f;
    m_dto.id() = id;
    const auto bb = BoundingVolume{ MathLib::Sphere3::UNIT_SPHERE };
    m_dto.geometryBound() = bb.serializeDto().toGenericDto();
}

SphereDtoHelper& SphereDtoHelper::sphere(const MathLib::Vector3& center, float radius, int slices, int stacks)
{
    m_center = center;
    m_radius = radius;

    // copy from WildMagic5, StandardMesh::Sphere
    // 原程式是右手座標系的, 要改左手, z軸改為y軸, 三角形頂點順序調整
    // 座標位置加上sphere center
    if (stacks < 4) stacks = 4;
    unsigned int zsm1 = stacks - 1, zsm2 = stacks - 2, zsm3 = stacks - 3;
    int rsp1 = slices + 1;
    int numVertices = static_cast<int>(zsm2) * rsp1 + 2;
    int numTriangles = 2 * static_cast<int>(zsm2) * slices;
    int numIndices = 3 * numTriangles;

    std::vector<MathLib::Vector3> vecPos(numVertices);
    std::vector<MathLib::Vector3> vecNor(numVertices);
    std::vector<MathLib::Vector2> uvTex(numVertices);
    uint_buffer vtxIndex(numIndices);

    // Generate geometry.
    float invRS = 1.0f / static_cast<float>(slices);
    float zFactor = 2.0f / static_cast<float>(zsm1);
    unsigned int r, z, zStart, i;
    MathLib::Vector2 tcoord;

    // Generate points on the unit circle to be used in computing the mesh
    // points on a cylinder slice.
    std::vector<float> sn;
    sn.resize(rsp1);
    std::vector<float> cs;
    cs.resize(rsp1);
    for (r = 0; r < static_cast<unsigned>(slices); ++r)
    {
        float angle = MathLib::Math::TWO_PI * invRS * static_cast<float>(r);
        cs[r] = std::cos(angle);
        sn[r] = std::sin(angle);
    }
    sn[slices] = sn[0];
    cs[slices] = cs[0];

    // Generate the cylinder itself.
    for (z = 1, i = 0; z < zsm1; ++z)
    {
        float zFraction = -1.0f + zFactor * static_cast<float>(z);  // in (-1,1)
        float zValue = radius * zFraction;

        // Compute center of slice.
        MathLib::Vector3 sliceCenter(0.0f, zValue, 0.0f);

        // Compute radius of slice.
        float sliceRadius = std::sqrt(std::fabs(radius * radius - zValue * zValue));

        // Compute slice vertices with duplication at endpoint.
        MathLib::Vector3 normal;
        int save = static_cast<int>(i);
        for (r = 0; r < static_cast<unsigned>(slices); ++r)
        {
            float radialFraction = static_cast<float>(r) * invRS;  // in [0,1)
            MathLib::Vector3 radial(sn[r], 0.0f, cs[r]);

            vecPos[i] = sliceCenter + sliceRadius * radial;

            // 球的中心點在(0,0,0), normal 就跟位置是一樣的
            normal = vecPos[i];
            vecNor[i] = normal.normalize();
            vecPos[i] += center;

            tcoord[0] = radialFraction;
            tcoord[1] = 0.5f * (zFraction + 1.0f);
            uvTex[i] = tcoord;

            ++i;
        }

        vecPos[i] = vecPos[save];
        vecNor[i] = vecNor[save];

        tcoord[0] = 1.0f;
        tcoord[1] = 0.5f * (zFraction + 1.0f);
        uvTex[i] = tcoord;

        ++i;
    }

    // south pole
    vecPos[i] = MathLib::Vector3(0.0f, -radius, 0.0f);
    vecNor[i] = MathLib::Vector3(0.0f, -1.0f, 0.0f);
    vecPos[i] += center;

    tcoord = MathLib::Vector2(0.5f, 0.5f);
    uvTex[i] = tcoord;

    ++i;

    // north pole
    vecPos[i] = MathLib::Vector3(0.0f, radius, 0.0f);
    vecNor[i] = MathLib::Vector3(0.0f, 1.0f, 0.0f);
    vecPos[i] += center;

    tcoord = MathLib::Vector2(0.5f, 1.0f);
    uvTex[i] = tcoord;

    ++i;

    // Generate indices.
    unsigned int* indices = &vtxIndex[0];
    for (z = 0, zStart = 0; z < zsm3; ++z)
    {
        unsigned int i0 = zStart;
        unsigned int i1 = i0 + 1;
        zStart += rsp1;
        unsigned int i2 = zStart;
        unsigned int i3 = i2 + 1;
        for (i = 0; i < static_cast<unsigned>(slices); ++i, indices += 6)
        {
            indices[0] = i0++;
            indices[1] = i2;
            indices[2] = i1;
            indices[3] = i1++;
            indices[4] = i2++;
            indices[5] = i3++;
        }
    }

    // south pole triangles
    int numVerticesM2 = numVertices - 2;
    for (i = 0; i < static_cast<unsigned>(slices); ++i, indices += 3)
    {
        indices[0] = i;
        indices[2] = i + 1;
        indices[1] = numVerticesM2;
    }

    // north pole triangles
    int numVerticesM1 = numVertices - 1, offset = static_cast<int>(zsm3) * rsp1;
    for (i = 0; i < static_cast<unsigned>(slices); ++i, indices += 3)
    {
        indices[0] = i + offset;
        indices[2] = numVerticesM1;
        indices[1] = i + 1 + offset;
    }

    m_dto.position3s() = vecPos;
    m_dto.indices() = vtxIndex;
    m_dto.segments() = { 0, static_cast<unsigned>(numVertices), 0, static_cast<unsigned>(numIndices) };
    m_dto.vertexUsedCount() = static_cast<unsigned>(numVertices);
    m_dto.indexUsedCount() = static_cast<unsigned>(numIndices);
    m_dto.vertexCapacity() = static_cast<unsigned>(numVertices);
    m_dto.indexCapacity() = static_cast<unsigned>(numIndices);
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;

    m_normals = vecNor;
    m_tex_coords = uvTex;

    return *this;
}

SphereDtoHelper& SphereDtoHelper::normal()
{
    m_dto.normals() = m_normals;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    return *this;
}

SphereDtoHelper& SphereDtoHelper::textureCoord()
{
    size_t tex_stage_count = m_dto.textureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.texture2DCoords() = m_tex_coords;
    m_dto.textureCoords().emplace_back(tex_dto.toGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 2;
    m_format.m_texCount = static_cast<unsigned>(m_dto.textureCoords().size());
    return *this;
}

SphereDtoHelper& SphereDtoHelper::sphereBound()
{
    const auto bb = BoundingVolume{ MathLib::Sphere3(m_center, m_radius) };
    m_dto.geometryBound() = bb.serializeDto().toGenericDto();
    return *this;
}

SphereDtoHelper& SphereDtoHelper::boxBound()
{
    MathLib::Box3 box;
    box.Center() = m_center;
    box.Extent(0) = m_radius;
    box.Extent(1) = m_radius;
    box.Extent(2) = m_radius;

    const auto bb = BoundingVolume{ box };
    m_dto.geometryBound() = bb.serializeDto().toGenericDto();
    return *this;
}

SphereDtoHelper& SphereDtoHelper::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_dto.factoryDesc().ClaimAsResourceAsset(name, filename, path_id);
    return *this;
}

GenericDto SphereDtoHelper::toGenericDto()
{
    m_dto.vertexFormat() = m_format.ToString();
    m_dto.topology() = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    return m_dto.toGenericDto();
}


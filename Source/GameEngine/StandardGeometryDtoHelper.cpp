#include "StandardGeometryDtoHelper.h"
#include "BoundingVolume.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/ContainmentBox3.h"
#include "MathLib/MathGlobal.h"

using namespace Enigma::Engine;

SquareQuadDtoHelper::SquareQuadDtoHelper(const std::string& name)
{
    m_dto.Name() = name;
}

SquareQuadDtoHelper& SquareQuadDtoHelper::XYQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top)
{
    std::vector<MathLib::Vector3> positions;
    positions.emplace_back(left_bottom);
    positions.emplace_back(MathLib::Vector3(left_bottom.X(), right_top.Y(), left_bottom.Z()));
    positions.emplace_back(MathLib::Vector3(right_top.X(), right_top.Y(), left_bottom.Z()));
    positions.emplace_back(MathLib::Vector3(right_top.X(), left_bottom.Y(), left_bottom.Z()));
    m_dto.Position3s() = positions;
    m_normal = MathLib::Vector3(0, 0, 1);
    uint_buffer indices =
    {
        0,1,2, 0,2,3
    };
    m_dto.Indices() = indices;
    m_dto.Segments() = { 0, 4, 0, 6 };
    m_dto.VertexUsedCount() = 4;
    m_dto.IndexUsedCount() = 6;
    m_dto.VertexCapacity() = 4;
    m_dto.IndexCapacity() = 6;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;
    return *this;
}

SquareQuadDtoHelper& SquareQuadDtoHelper::XZQuad(const MathLib::Vector3& left_bottom, const MathLib::Vector3& right_top)
{
    std::vector<MathLib::Vector3> positions;
    positions.emplace_back(left_bottom);
    positions.emplace_back(MathLib::Vector3(left_bottom.X(), left_bottom.Y(), right_top.Z()));
    positions.emplace_back(MathLib::Vector3(right_top.X(), left_bottom.Y(), right_top.Z()));
    positions.emplace_back(MathLib::Vector3(right_top.X(), left_bottom.Y(), left_bottom.Z()));
    m_dto.Position3s() = positions;
    m_normal = MathLib::Vector3(0, 1, 0);
    uint_buffer indices =
    {
        0,1,2, 0,2,3
    };
    m_dto.Indices() = indices;
    m_dto.Segments() = { 0, 4, 0, 6 };
    m_dto.VertexUsedCount() = 4;
    m_dto.IndexUsedCount() = 6;
    m_dto.VertexCapacity() = 4;
    m_dto.IndexCapacity() = 6;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;
    return *this;

}

SquareQuadDtoHelper& SquareQuadDtoHelper::Normal()
{
    std::vector<MathLib::Vector3> normals;
    normals.emplace_back(m_normal);
    normals.emplace_back(m_normal);
    normals.emplace_back(m_normal);
    normals.emplace_back(m_normal);
    m_dto.Normals() = normals;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    return *this;
}

SquareQuadDtoHelper& SquareQuadDtoHelper::TextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    std::vector<MathLib::Vector2> texcoords;
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), right_top.Y()));
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    size_t tex_stage_count = m_dto.TextureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.Texture2DCoords() = texcoords;
    m_dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 2;
    m_format.m_texCount = static_cast<unsigned>(m_dto.TextureCoords().size());
    return *this;
}

GenericDto SquareQuadDtoHelper::ToGenericDto()
{
    assert(m_dto.Position3s() || m_dto.Position4s());
    m_dto.VertexFormat() = m_format.ToString();
    m_dto.Topology() = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    if (auto& pos3 = m_dto.Position3s())
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos3.value()[0]), static_cast<unsigned>(pos3.value().size()));
        const auto bb = BoundingVolume{ box };
        m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    }
    else if (auto& pos4 = m_dto.Position4s())
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos4.value()[0]), static_cast<unsigned>(pos4.value().size()));
        const auto bb = BoundingVolume{ box };
        m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    }
    return m_dto.ToGenericDto();
}

CubeDtoHelper::CubeDtoHelper(const std::string& name)
{
    m_dto.Name() = name;
}

CubeDtoHelper& CubeDtoHelper::Cube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent)
{
    std::vector<MathLib::Vector3> positions;
    MathLib::Vector3 xyz0 = center - axis_extent;
    MathLib::Vector3 xyz1 = center + axis_extent;
    positions.emplace_back(xyz0);
    positions.emplace_back(MathLib::Vector3(xyz1.X(), xyz0.Y(), xyz0.Z()));
    positions.emplace_back(MathLib::Vector3(xyz1.X(), xyz0.Y(), xyz1.Z()));
    positions.emplace_back(MathLib::Vector3(xyz0.X(), xyz0.Y(), xyz1.Z()));
    positions.emplace_back(MathLib::Vector3(xyz0.X(), xyz1.Y(), xyz0.Z()));
    positions.emplace_back(MathLib::Vector3(xyz1.X(), xyz1.Y(), xyz0.Z()));
    positions.emplace_back(xyz1);
    positions.emplace_back(MathLib::Vector3(xyz0.X(), xyz1.Y(), xyz1.Z()));
    m_dto.Position3s() = positions;
    uint_buffer indices =
    {
        0, 1, 2, 0, 2, 3,

        0, 5, 1, 0, 4, 5,
        1, 6, 2, 1, 5, 6,
        2, 7, 3, 2, 6, 7,
        3, 4, 0, 3, 7, 4,

        4, 6, 5, 4, 7, 6
    };
    m_dto.Indices() = indices;
    m_dto.Segments() = { 0, 8, 0, 36 };
    m_dto.VertexUsedCount() = 8;
    m_dto.IndexUsedCount() = 36;
    m_dto.VertexCapacity() = 8;
    m_dto.IndexCapacity() = 36;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;
    return *this;
}

CubeDtoHelper& CubeDtoHelper::FacedCube(const MathLib::Vector3& center, const MathLib::Vector3& axis_extent)
{
    std::vector<MathLib::Vector3> positions;
    MathLib::Vector3 xyz0 = center - axis_extent;
    MathLib::Vector3 xyz6 = center + axis_extent;
    MathLib::Vector3 xyz1(xyz6.X(), xyz0.Y(), xyz0.Z());
    MathLib::Vector3 xyz2(xyz6.X(), xyz0.Y(), xyz6.Z());
    MathLib::Vector3 xyz3(xyz0.X(), xyz0.Y(), xyz6.Z());
    MathLib::Vector3 xyz4(xyz0.X(), xyz6.Y(), xyz0.Z());
    MathLib::Vector3 xyz5(xyz6.X(), xyz6.Y(), xyz0.Z());
    MathLib::Vector3 xyz7(xyz0.X(), xyz6.Y(), xyz6.Z());
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
    m_dto.Position3s() = positions;
    uint_buffer indices =
    {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23,
    };
    m_dto.Indices() = indices;
    m_dto.Segments() = { 0, 24, 0, 36 };
    m_dto.VertexUsedCount() = 24;
    m_dto.IndexUsedCount() = 36;
    m_dto.VertexCapacity() = 24;
    m_dto.IndexCapacity() = 36;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;
    return *this;
}

CubeDtoHelper& CubeDtoHelper::Normal()
{
    std::vector<MathLib::Vector3> normals;
    normals.emplace_back(MathLib::Vector3(-1, -1, -1).Normalize());
    normals.emplace_back(MathLib::Vector3(1, -1, -1).Normalize());
    normals.emplace_back(MathLib::Vector3(1, -1, 1).Normalize());
    normals.emplace_back(MathLib::Vector3(-1, -1, 1).Normalize());
    normals.emplace_back(MathLib::Vector3(-1, 1, -1).Normalize());
    normals.emplace_back(MathLib::Vector3(1, 1, -1).Normalize());
    normals.emplace_back(MathLib::Vector3(1, 1, 1).Normalize());
    normals.emplace_back(MathLib::Vector3(-1, 1, 1).Normalize());
    m_dto.Normals() = normals;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    return *this;
}

CubeDtoHelper& CubeDtoHelper::FacedNormal()
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
    m_dto.Normals() = normals;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    return *this;
}

CubeDtoHelper& CubeDtoHelper::TextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    std::vector<MathLib::Vector2> texcoords;
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), right_top.Y()));
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), right_top.Y()));
    size_t tex_stage_count = m_dto.TextureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.Texture2DCoords() = texcoords;
    m_dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 2;
    m_format.m_texCount = static_cast<unsigned>(m_dto.TextureCoords().size());
    return *this;
}

CubeDtoHelper& CubeDtoHelper::FacedTextureCoord(const MathLib::Vector2& left_bottom, const MathLib::Vector2& right_top)
{
    std::vector<MathLib::Vector2> texcoords;
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    texcoords.emplace_back(left_bottom);
    texcoords.emplace_back(MathLib::Vector2(left_bottom.X(), right_top.Y()));
    texcoords.emplace_back(right_top);
    texcoords.emplace_back(MathLib::Vector2(right_top.X(), left_bottom.Y()));
    size_t tex_stage_count = m_dto.TextureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.Texture2DCoords() = texcoords;
    m_dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 2;
    m_format.m_texCount = static_cast<unsigned>(m_dto.TextureCoords().size());
    return *this;
}

CubeDtoHelper& CubeDtoHelper::TextureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back)
{
    std::vector<MathLib::Vector3> texcoords;
    texcoords.emplace_back(left_bottom_front);
    texcoords.emplace_back(MathLib::Vector3(right_top_back.X(), left_bottom_front.Y(), left_bottom_front.Z()));
    texcoords.emplace_back(MathLib::Vector3(right_top_back.X(), left_bottom_front.Y(), right_top_back.Z()));
    texcoords.emplace_back(MathLib::Vector3(left_bottom_front.X(), left_bottom_front.Y(), right_top_back.Z()));
    texcoords.emplace_back(MathLib::Vector3(left_bottom_front.X(), right_top_back.Y(), left_bottom_front.Z()));
    texcoords.emplace_back(MathLib::Vector3(right_top_back.X(), right_top_back.Y(), left_bottom_front.Z()));
    texcoords.emplace_back(MathLib::Vector3(right_top_back));
    texcoords.emplace_back(MathLib::Vector3(left_bottom_front.X(), right_top_back.Y(), right_top_back.Z()));
    size_t tex_stage_count = m_dto.TextureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.Texture3DCoords() = texcoords;
    m_dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 3;
    m_format.m_texCount = static_cast<unsigned>(m_dto.TextureCoords().size());
    return *this;
}

CubeDtoHelper& CubeDtoHelper::FacedTextureCoord(const MathLib::Vector3& left_bottom_front, const MathLib::Vector3& right_top_back)
{
    std::vector<MathLib::Vector3> texcoords;
    MathLib::Vector3 xyz0 = left_bottom_front;
    MathLib::Vector3 xyz6 = right_top_back;
    MathLib::Vector3 xyz1(xyz6.X(), xyz0.Y(), xyz0.Z());
    MathLib::Vector3 xyz2(xyz6.X(), xyz0.Y(), xyz6.Z());
    MathLib::Vector3 xyz3(xyz0.X(), xyz0.Y(), xyz6.Z());
    MathLib::Vector3 xyz4(xyz0.X(), xyz6.Y(), xyz0.Z());
    MathLib::Vector3 xyz5(xyz6.X(), xyz6.Y(), xyz0.Z());
    MathLib::Vector3 xyz7(xyz0.X(), xyz6.Y(), xyz6.Z());
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
    size_t tex_stage_count = m_dto.TextureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.Texture3DCoords() = texcoords;
    m_dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 3;
    m_format.m_texCount = static_cast<unsigned>(m_dto.TextureCoords().size());
    return *this;
}

GenericDto CubeDtoHelper::ToGenericDto()
{
    assert(m_dto.Position3s() || m_dto.Position4s());
    m_dto.VertexFormat() = m_format.ToString();
    m_dto.Topology() = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    if (auto& pos = m_dto.Position3s())
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos.value()[0]), static_cast<unsigned>(pos.value().size()));
        const auto bb = BoundingVolume{ box };
        m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    }
    else if (auto& pos4 = m_dto.Position4s())
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos4.value()[0]), static_cast<unsigned>(pos4.value().size()));
        const auto bb = BoundingVolume{ box };
        m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    }
    return m_dto.ToGenericDto();
}

SphereDtoHelper::SphereDtoHelper(const std::string& name)
{
    m_radius = 1.0f;
    m_dto.Name() = name;
    const auto bb = BoundingVolume{ MathLib::Sphere3::UNIT_SPHERE };
    m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
}

SphereDtoHelper& SphereDtoHelper::Sphere(const MathLib::Vector3& center, float radius, int slices, int stacks)
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
            vecNor[i] = normal.Normalize();
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

    m_dto.Position3s() = vecPos;
    m_dto.Indices() = vtxIndex;
    m_dto.Segments() = { 0, static_cast<unsigned>(numVertices), 0, static_cast<unsigned>(numIndices) };
    m_dto.VertexUsedCount() = static_cast<unsigned>(numVertices);
    m_dto.IndexUsedCount() = static_cast<unsigned>(numIndices);
    m_dto.VertexCapacity() = static_cast<unsigned>(numVertices);
    m_dto.IndexCapacity() = static_cast<unsigned>(numIndices);
    m_format.m_fvfCode |= Graphics::VertexFormatCode::XYZ;

    m_normals = vecNor;
    m_tex_coords = uvTex;

    return *this;
}

SphereDtoHelper& SphereDtoHelper::Normal()
{
    m_dto.Normals() = m_normals;
    m_format.m_fvfCode |= Graphics::VertexFormatCode::NORMAL;
    return *this;
}

SphereDtoHelper& SphereDtoHelper::TextureCoord()
{
    size_t tex_stage_count = m_dto.TextureCoords().size();
    TextureCoordDto tex_dto;
    tex_dto.Texture2DCoords() = m_tex_coords;
    m_dto.TextureCoords().emplace_back(tex_dto.ToGenericDto());
    m_format.m_texCoordSize[tex_stage_count] = 2;
    m_format.m_texCount = static_cast<unsigned>(m_dto.TextureCoords().size());
    return *this;
}

SphereDtoHelper& SphereDtoHelper::SphereBound()
{
    const auto bb = BoundingVolume{ MathLib::Sphere3(m_center, m_radius) };
    m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    return *this;
}

SphereDtoHelper& SphereDtoHelper::BoxBound()
{
    MathLib::Box3 box;
    box.Center() = m_center;
    box.Extent(0) = m_radius;
    box.Extent(1) = m_radius;
    box.Extent(2) = m_radius;

    const auto bb = BoundingVolume{ box };
    m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    return *this;
}

GenericDto SphereDtoHelper::ToGenericDto()
{
    m_dto.VertexFormat() = m_format.ToString();
    m_dto.Topology() = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    return m_dto.ToGenericDto();
}


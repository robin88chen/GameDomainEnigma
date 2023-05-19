#include "StandardGeometryDtoHelper.h"
#include "BoundingVolume.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "MathLib/ContainmentBox3.h"

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
    normals.emplace_back(MathLib::Vector3(0, 0, 1));
    normals.emplace_back(MathLib::Vector3(0, 0, 1));
    normals.emplace_back(MathLib::Vector3(0, 0, 1));
    normals.emplace_back(MathLib::Vector3(0, 0, 1));
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
    m_format.m_texCount = m_dto.TextureCoords().size();
    return *this;
}

GenericDto SquareQuadDtoHelper::ToGenericDto()
{
    assert(m_dto.Position3s() || m_dto.Position4s());
    m_dto.VertexFormat() = m_format.ToString();
    m_dto.Topology() = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    if (m_dto.Position3s())
    {
        MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(m_dto.Position3s().value()[0]), static_cast<unsigned>(m_dto.Position3s().value().size()));
        auto bb = BoundingVolume{ box };
        m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    }
    else if (m_dto.Position4s())
    {
        MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(m_dto.Position4s().value()[0]), static_cast<unsigned>(m_dto.Position4s().value().size()));
        auto bb = BoundingVolume{ box };
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
    m_format.m_texCount = m_dto.TextureCoords().size();
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
    m_format.m_texCount = m_dto.TextureCoords().size();
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
    m_format.m_texCount = m_dto.TextureCoords().size();
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
    m_format.m_texCount = m_dto.TextureCoords().size();
    return *this;
}

GenericDto CubeDtoHelper::ToGenericDto()
{
    assert(m_dto.Position3s() || m_dto.Position4s());
    m_dto.VertexFormat() = m_format.ToString();
    m_dto.Topology() = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    if (m_dto.Position3s())
    {
        MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(m_dto.Position3s().value()[0]), static_cast<unsigned>(m_dto.Position3s().value().size()));
        auto bb = BoundingVolume{ box };
        m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    }
    else if (m_dto.Position4s())
    {
        MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(m_dto.Position4s().value()[0]), static_cast<unsigned>(m_dto.Position4s().value().size()));
        auto bb = BoundingVolume{ box };
        m_dto.GeometryBound() = bb.SerializeDto().ToGenericDto();
    }
    return m_dto.ToGenericDto();
}

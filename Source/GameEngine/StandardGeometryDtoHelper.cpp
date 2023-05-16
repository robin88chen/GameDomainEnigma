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


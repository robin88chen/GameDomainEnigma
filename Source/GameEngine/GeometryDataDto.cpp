#include "GeometryDataDto.h"
#include "GraphicKernel/VertexDescription.h"
#include "GeometryData.h"
#include "TriangleList.h"
#include <cassert>

using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Graphics;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_VERTEX_FORMAT = "VertexFormat";
static std::string TOKEN_SEGMENTS = "Segments";
static std::string TOKEN_POSITIONS_3 = "Positions3";
static std::string TOKEN_POSITIONS_4 = "Positions4";
static std::string TOKEN_NORMALS = "Normals";
static std::string TOKEN_DIFFUSE_COLORS = "DiffuseColors";
static std::string TOKEN_SPECULAR_COLORS = "SpecularColors";
static std::string TOKEN_TEX_COORD0 = "TexCoord0";
static std::string TOKEN_TEX_COORD1 = "TexCoord1";
static std::string TOKEN_TEX_COORD2 = "TexCoord2";
static std::string TOKEN_TEX_COORD3 = "TexCoord3";
static std::string TOKEN_TEX_COORD4 = "TexCoord4";
static std::string TOKEN_TEX_COORD5 = "TexCoord5";
static std::string TOKEN_TEX_COORD6 = "TexCoord6";
static std::string TOKEN_TEX_COORD7 = "TexCoord7";
static std::string TOKEN_PALETTE_INDICES = "PaletteIndices";
static std::string TOKEN_WEIGHTS = "Weights";
static std::string TOKEN_TANGENTS = "Tangents";
static std::string TOKEN_INDICES = "Indices";
static std::string TOKEN_VERTEX_CAPACITY = "VertexCapacity";
static std::string TOKEN_INDEX_CAPACITY = "IndexCapacity";
static std::string TOKEN_VERTEX_USED_COUNT = "VertexUsedCount";
static std::string TOKEN_INDEX_USED_COUNT = "IndexUsedCount";
static std::string TOKEN_TOPOLOGY = "Topology";
static std::string TOKEN_GEOMETRY_BOUND = "GeometryBound";
static std::string TOKEN_2D_COORDS = "2DCoords";
static std::string TOKEN_1D_COORDS = "1DCoords";
static std::string TOKEN_3D_COORDS = "3DCoords";
static std::array<std::string, VertexFormatCode::MAX_TEX_COORD> TOKEN_TEX_COORDS =
{
    TOKEN_TEX_COORD0, TOKEN_TEX_COORD1, TOKEN_TEX_COORD2, TOKEN_TEX_COORD3,
    TOKEN_TEX_COORD4, TOKEN_TEX_COORD5, TOKEN_TEX_COORD6, TOKEN_TEX_COORD7
};

TextureCoordDto TextureCoordDto::FromGenericDto(const GenericDto& dto)
{
    TextureCoordDto coords;
    if (dto.HasValue(TOKEN_2D_COORDS))
    {
       if (auto v = dto.TryGetValue<std::vector<Vector2>>(TOKEN_2D_COORDS)) coords.m_2dCoords = v.value();
    }
    else if (dto.HasValue(TOKEN_1D_COORDS))
    {
        if (auto v = dto.TryGetValue<std::vector<float>>(TOKEN_1D_COORDS)) coords.m_1dCoords = v.value();
    }
    else if (dto.HasValue(TOKEN_3D_COORDS))
    {
        if (auto v = dto.TryGetValue<std::vector<Vector3>>(TOKEN_3D_COORDS)) coords.m_3dCoords = v.value();
    }
    return coords;
}

GenericDto TextureCoordDto::ToGenericDto()
{
    GenericDto dto;
    if (m_2dCoords)
    {
        dto.AddOrUpdate(TOKEN_2D_COORDS, m_2dCoords.value());
    }
    else if (m_1dCoords)
    {
        dto.AddOrUpdate(TOKEN_1D_COORDS, m_1dCoords.value());
    }
    else if (m_3dCoords)
    {
        dto.AddOrUpdate(TOKEN_3D_COORDS, m_3dCoords.value());
    }
    return dto;
}

//-----------------------------------------------------------------------------------
GeometryDataDto::GeometryDataDto() : m_vtxCapacity(0), m_idxCapacity(0), m_vtxUsedCount(0), m_idxUsedCount(0), m_topology(0), m_factoryDesc(GeometryData::TYPE_RTTI.getName())
{
}

GeometryDataDto GeometryDataDto::FromGenericDto(const GenericDto& dto)
{
    GeometryDataDto geometry;
    geometry.TheFactoryDesc() = dto.GetRtti();
    geometry.DeserializeNonVertexAttributesFromGenericDto(dto);
    if (dto.HasValue(TOKEN_POSITIONS_3))
    {
        if (auto v = dto.TryGetValue<std::vector<Vector3>>(TOKEN_POSITIONS_3)) geometry.m_position3s = v.value();
    }
    if (dto.HasValue(TOKEN_POSITIONS_4))
    {
        if (auto v = dto.TryGetValue<std::vector<Vector4>>(TOKEN_POSITIONS_4)) geometry.m_position4s = v.value();
    }
    if (dto.HasValue(TOKEN_NORMALS))
    {
        if (auto v = dto.TryGetValue<std::vector<Vector3>>(TOKEN_NORMALS)) geometry.m_normals = v.value();
    }
    if (dto.HasValue(TOKEN_DIFFUSE_COLORS))
    {
        if (auto v = dto.TryGetValue<std::vector<Vector4>>(TOKEN_DIFFUSE_COLORS)) geometry.m_diffuseColors = v.value();
    }
    if (dto.HasValue(TOKEN_SPECULAR_COLORS))
    {
        if (auto v = dto.TryGetValue<std::vector<Vector4>>(TOKEN_SPECULAR_COLORS)) geometry.m_specularColors = v.value();
    }
    for (auto& token_tex_coord : TOKEN_TEX_COORDS)
    {
        if (dto.HasValue(token_tex_coord))
        {
            if (auto v = dto.TryGetValue<GenericDto>(token_tex_coord)) geometry.m_texCoords.emplace_back(v.value());
        }
    }
    if (dto.HasValue(TOKEN_PALETTE_INDICES))
    {
        if (auto v = dto.TryGetValue<std::vector<unsigned>>(TOKEN_PALETTE_INDICES)) geometry.m_paletteIndices = v.value();
    }
    if (dto.HasValue(TOKEN_WEIGHTS))
    {
        if (auto v = dto.TryGetValue<std::vector<float>>(TOKEN_WEIGHTS)) geometry.m_weights = v.value();
    }
    if (dto.HasValue(TOKEN_TANGENTS))
    {
        if (auto v = dto.TryGetValue<std::vector<Vector4>>(TOKEN_TANGENTS)) geometry.m_tangents = v.value();
    }
    if (dto.HasValue(TOKEN_INDICES))
    {
        if (auto v = dto.TryGetValue<std::vector<unsigned>>(TOKEN_INDICES)) geometry.m_indices = v.value();
    }

    return geometry;
}

GenericDto GeometryDataDto::ToGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    SerializeNonVertexAttributesToGenericDto(dto);
    if (m_position3s)
    {
        dto.AddOrUpdate(TOKEN_POSITIONS_3, m_position3s.value());
    }
    if (m_position4s)
    {
        dto.AddOrUpdate(TOKEN_POSITIONS_4, m_position4s.value());
    }
    if (m_normals)
    {
        dto.AddOrUpdate(TOKEN_NORMALS, m_normals.value());
    }
    if (m_diffuseColors)
    {
        dto.AddOrUpdate(TOKEN_DIFFUSE_COLORS, m_diffuseColors.value());
    }
    if (m_specularColors)
    {
        dto.AddOrUpdate(TOKEN_SPECULAR_COLORS, m_specularColors.value());
    }
    for (unsigned i = 0; i < m_texCoords.size(); i++)
    {
        dto.AddOrUpdate(TOKEN_TEX_COORDS[i], m_texCoords[i]);
    }
    if (m_paletteIndices)
    {
        dto.AddOrUpdate(TOKEN_PALETTE_INDICES, m_paletteIndices.value());
    }
    if (m_weights)
    {
        dto.AddOrUpdate(TOKEN_WEIGHTS, m_weights.value());
    }
    if (m_tangents)
    {
        dto.AddOrUpdate(TOKEN_TANGENTS, m_tangents.value());
    }
    if (m_indices)
    {
        dto.AddOrUpdate(TOKEN_INDICES, m_indices.value());
    }

    return dto;
}

void GeometryDataDto::DeserializeNonVertexAttributesFromGenericDto(const GenericDto& dto)
{
    if (auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) m_name = v.value();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_VERTEX_FORMAT)) m_vertexFormat = v.value();
    if (auto v = dto.TryGetValue<std::vector<unsigned>>(TOKEN_SEGMENTS)) m_segments = v.value();
    if (auto v = dto.TryGetValue<unsigned>(TOKEN_VERTEX_CAPACITY)) m_vtxCapacity = v.value();
    if (auto v = dto.TryGetValue<unsigned>(TOKEN_INDEX_CAPACITY)) m_idxCapacity = v.value();
    if (auto v = dto.TryGetValue<unsigned>(TOKEN_VERTEX_USED_COUNT)) m_vtxUsedCount = v.value();
    if (auto v = dto.TryGetValue<unsigned>(TOKEN_INDEX_USED_COUNT)) m_idxUsedCount = v.value();
    if (auto v = dto.TryGetValue<unsigned>(TOKEN_TOPOLOGY)) m_topology = v.value();
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_GEOMETRY_BOUND)) m_geometryBound = v.value();
}

void GeometryDataDto::SerializeNonVertexAttributesToGenericDto(GenericDto& dto) const
{
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_VERTEX_FORMAT, m_vertexFormat);
    dto.AddOrUpdate(TOKEN_SEGMENTS, m_segments);
    dto.AddOrUpdate(TOKEN_VERTEX_CAPACITY, m_vtxCapacity);
    dto.AddOrUpdate(TOKEN_INDEX_CAPACITY, m_idxCapacity);
    dto.AddOrUpdate(TOKEN_VERTEX_USED_COUNT, m_vtxUsedCount);
    dto.AddOrUpdate(TOKEN_INDEX_USED_COUNT, m_idxUsedCount);
    dto.AddOrUpdate(TOKEN_TOPOLOGY, m_topology);
    dto.AddOrUpdate(TOKEN_GEOMETRY_BOUND, m_geometryBound);
}

TriangleListDto::TriangleListDto() : GeometryDataDto()
{
    m_factoryDesc = FactoryDesc(TriangleList::TYPE_RTTI.getName());
}

TriangleListDto::TriangleListDto(const GeometryDataDto& geometry_dto) : GeometryDataDto(geometry_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), TriangleList::TYPE_RTTI.getName()));
}

TriangleListDto TriangleListDto::FromGenericDto(const Engine::GenericDto& dto)
{
    return TriangleListDto(GeometryDataDto::FromGenericDto(dto));
}

GenericDto TriangleListDto::ToGenericDto() const
{
    GenericDto dto = GeometryDataDto::ToGenericDto();
    //dto.AddRtti(FactoryDesc(TriangleList::TYPE_RTTI.getName()));

    return dto;
}

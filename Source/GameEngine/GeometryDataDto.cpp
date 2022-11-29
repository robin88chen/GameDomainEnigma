#include "GeometryDataDto.h"
#include "GraphicKernel/VertexDescription.h"
#include "GeometryData.h"
#include "TriangleList.h"

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
        coords.m_2dCoords = dto.TryGetValue<std::vector<Vector2>>(TOKEN_2D_COORDS).value();
    }
    else if (dto.HasValue(TOKEN_1D_COORDS))
    {
        coords.m_1dCoords = dto.TryGetValue<std::vector<float>>(TOKEN_1D_COORDS).value();
    }
    else if (dto.HasValue(TOKEN_3D_COORDS))
    {
        coords.m_3dCoords = dto.TryGetValue<std::vector<Vector3>>(TOKEN_3D_COORDS).value();
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
GeometryDataDto GeometryDataDto::FromGenericDto(const GenericDto& dto)
{
    GeometryDataDto geometry;
    geometry.m_name = dto.TryGetValue<std::string>(TOKEN_NAME).value();
    geometry.m_vertexFormat = dto.TryGetValue<std::string>(TOKEN_VERTEX_FORMAT).value();
    geometry.m_segments = dto.TryGetValue<std::vector<unsigned>>(TOKEN_SEGMENTS).value();
    if (dto.HasValue(TOKEN_POSITIONS_3))
    {
        geometry.m_position3s = dto.TryGetValue<std::vector<Vector3>>(TOKEN_POSITIONS_3).value();
    }
    if (dto.HasValue(TOKEN_POSITIONS_4))
    {
        geometry.m_position4s = dto.TryGetValue<std::vector<Vector4>>(TOKEN_POSITIONS_4).value();
    }
    if (dto.HasValue(TOKEN_NORMALS))
    {
        geometry.m_normals = dto.TryGetValue<std::vector<Vector3>>(TOKEN_NORMALS).value();
    }
    if (dto.HasValue(TOKEN_DIFFUSE_COLORS))
    {
        geometry.m_diffuseColors = dto.TryGetValue<std::vector<Vector4>>(TOKEN_DIFFUSE_COLORS).value();
    }
    if (dto.HasValue(TOKEN_SPECULAR_COLORS))
    {
        geometry.m_specularColors = dto.TryGetValue<std::vector<Vector4>>(TOKEN_SPECULAR_COLORS).value();
    }
    for (auto& token_tex_coord : TOKEN_TEX_COORDS)
    {
        if (dto.HasValue(token_tex_coord))
        {
            geometry.m_texCoords.emplace_back(dto.TryGetValue<GenericDto>(token_tex_coord).value());
        }
    }
    if (dto.HasValue(TOKEN_PALETTE_INDICES))
    {
        geometry.m_paletteIndices = dto.TryGetValue<std::vector<unsigned>>(TOKEN_PALETTE_INDICES).value();
    }
    if (dto.HasValue(TOKEN_WEIGHTS))
    {
        geometry.m_weights = dto.TryGetValue<std::vector<float>>(TOKEN_WEIGHTS).value();
    }
    if (dto.HasValue(TOKEN_TANGENTS))
    {
        geometry.m_tangents = dto.TryGetValue<std::vector<Vector4>>(TOKEN_TANGENTS).value();
    }
    if (dto.HasValue(TOKEN_INDICES))
    {
        geometry.m_indices = dto.TryGetValue<std::vector<unsigned>>(TOKEN_INDICES).value();
    }
    geometry.m_vtxCapacity = dto.TryGetValue<unsigned>(TOKEN_VERTEX_CAPACITY).value();
    geometry.m_idxCapacity = dto.TryGetValue<unsigned>(TOKEN_INDEX_CAPACITY).value();
    geometry.m_vtxUsedCount = dto.TryGetValue<unsigned>(TOKEN_VERTEX_USED_COUNT).value();
    geometry.m_idxUsedCount = dto.TryGetValue<unsigned>(TOKEN_INDEX_USED_COUNT).value();
    geometry.m_topology = dto.TryGetValue<unsigned>(TOKEN_TOPOLOGY).value();
    geometry.m_geometryBound = dto.TryGetValue<GenericDto>(TOKEN_GEOMETRY_BOUND).value();

    return geometry;
}

GenericDto GeometryDataDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(GeometryData::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_VERTEX_FORMAT, m_vertexFormat);
    dto.AddOrUpdate(TOKEN_SEGMENTS, m_segments);
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
    dto.AddOrUpdate(TOKEN_VERTEX_CAPACITY, m_vtxCapacity);
    dto.AddOrUpdate(TOKEN_INDEX_CAPACITY, m_idxCapacity);
    dto.AddOrUpdate(TOKEN_VERTEX_USED_COUNT, m_vtxUsedCount);
    dto.AddOrUpdate(TOKEN_INDEX_USED_COUNT, m_idxUsedCount);
    dto.AddOrUpdate(TOKEN_TOPOLOGY, m_topology);
    dto.AddOrUpdate(TOKEN_GEOMETRY_BOUND, m_geometryBound);

    return dto;
}

TriangleListDto::TriangleListDto(const GeometryDataDto& geometry_dto) : GeometryDataDto(geometry_dto)
{
}

TriangleListDto TriangleListDto::FromGenericDto(const Engine::GenericDto& dto)
{
    return TriangleListDto(GeometryDataDto::FromGenericDto(dto));
}

GenericDto TriangleListDto::ToGenericDto()
{
    GenericDto dto = GeometryDataDto::ToGenericDto();
    dto.AddRtti(FactoryDesc(TriangleList::TYPE_RTTI.GetName()));

    return dto;
}

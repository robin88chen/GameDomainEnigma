﻿#include "TerrainGeometryAssembler.h"
#include "TerrainGeometry.h"
#include "MathLib/ContainmentBox3.h"

using namespace Enigma::Terrain;
using namespace Enigma::Geometries;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

static std::string TOKEN_NUM_ROWS = "NumRows";
static std::string TOKEN_NUM_COLS = "NumCols";
static std::string TOKEN_MIN_POSITION = "MinPosition";
static std::string TOKEN_MAX_POSITION = "MaxPosition";
static std::string TOKEN_MIN_TEXTURE_COORDINATE = "MinTextureCoordinate";
static std::string TOKEN_MAX_TEXTURE_COORDINATE = "MaxTextureCoordinate";
static std::string TOKEN_HEIGHT_MAP = "HeightMap";

TerrainGeometryAssembler::TerrainGeometryAssembler(const GeometryId& id) : TriangleListAssembler(id)
{
    m_factoryDesc = FactoryDesc(TerrainGeometry::TYPE_RTTI);
    m_numRows = m_numCols = 1;
}

GenericDto TerrainGeometryAssembler::assemble()
{
    GenericDto dto;
    convertGeometryVertices();
    serializeNonVertexAttributesToGenericDto(dto);
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_NUM_ROWS, m_numRows);
    dto.addOrUpdate(TOKEN_NUM_COLS, m_numCols);
    dto.addOrUpdate(TOKEN_MIN_POSITION, m_minPosition);
    dto.addOrUpdate(TOKEN_MAX_POSITION, m_maxPosition);
    dto.addOrUpdate(TOKEN_MIN_TEXTURE_COORDINATE, m_minTextureCoordinate);
    dto.addOrUpdate(TOKEN_MAX_TEXTURE_COORDINATE, m_maxTextureCoordinate);
    if (m_heightMap)
    {
        dto.addOrUpdate(TOKEN_HEIGHT_MAP, m_heightMap.value());
    }
    return dto;
}

void TerrainGeometryAssembler::convertGeometryVertices()
{
    assert((m_numRows != 0) && (m_numCols != 0));
    Vector3 terrain_size = m_maxPosition - m_minPosition;
    Vector2 uv_range = m_maxTextureCoordinate - m_minTextureCoordinate;

    m_vtxCapacity = calculateGeometryVertexCount();
    m_vtxUsedCount = m_vtxCapacity;
    m_idxCapacity = calculateGeometryIndexCount();
    m_idxUsedCount = m_idxCapacity;
    m_segments = { 0, m_vtxUsedCount, 0, m_idxUsedCount };
    m_position3s = std::vector<Vector3>(m_vtxUsedCount);
    m_normals = std::vector<Vector3>(m_vtxUsedCount);
    std::vector<Vector2> tex_coords(m_vtxUsedCount);
    std::vector<Vector2> alpha_coords(m_vtxUsedCount);

    for (unsigned vi = 0; vi < m_vtxUsedCount; vi++)
    {
        unsigned xi = vi % (m_numCols + 1);
        unsigned zi = vi / (m_numCols + 1);
        float x_ratio = static_cast<float>(xi) / static_cast<float>(m_numCols + 1);
        float z_ratio = static_cast<float>(zi) / static_cast<float>(m_numRows + 1);
        float height = m_heightMap ? m_heightMap.value()[vi] : 0.0f;
        m_position3s.value()[vi] = Vector3(x_ratio * terrain_size.x() + m_minPosition.x(), height, z_ratio * terrain_size.z() + m_minPosition.z());
        m_normals.value()[vi] = Vector3(0.0f, 1.0f, 0.0f);
        tex_coords[vi] = Vector2(x_ratio * uv_range.x() + m_minTextureCoordinate.x(), (1.0f - z_ratio) * uv_range.y() + m_minTextureCoordinate.y());
        alpha_coords[vi] = Vector2(x_ratio, (1.0f - z_ratio));
    }

    m_indices = std::vector<unsigned int>(m_idxUsedCount);
    unsigned cell_count = m_numCols * m_numRows;
    for (unsigned cell_i = 0; cell_i < cell_count; cell_i++)
    {
        unsigned xi = cell_i % m_numCols;
        unsigned zi = cell_i / m_numRows;
        m_indices.value()[cell_i * 6] = zi * (m_numCols + 1) + xi;
        m_indices.value()[cell_i * 6 + 1] = (zi + 1) * (m_numCols + 1) + xi;
        m_indices.value()[cell_i * 6 + 2] = (zi + 1) * (m_numCols + 1) + xi + 1;
        m_indices.value()[cell_i * 6 + 3] = zi * (m_numCols + 1) + xi;
        m_indices.value()[cell_i * 6 + 4] = (zi + 1) * (m_numCols + 1) + xi + 1;
        m_indices.value()[cell_i * 6 + 5] = zi * (m_numCols + 1) + xi + 1;
    }
    TextureCoordinateAssembler tex_coord_assembler;
    TextureCoordinateAssembler alpha_coord_assembler;
    tex_coord_assembler.texture2DCoords(tex_coords);
    alpha_coord_assembler.texture2DCoords(alpha_coords);
    m_texCoords = { tex_coord_assembler.assemble(), alpha_coord_assembler.assemble() };

    m_vertexFormat.fromString("xyz_nor_tex2(2,2)");
    m_topology = static_cast<unsigned>(Graphics::PrimitiveTopology::Topology_TriangleList);
    m_geometryBound = calculateGeometryBounding();
}

unsigned TerrainGeometryAssembler::calculateGeometryVertexCount() const
{
    return (m_numRows + 1) * (m_numCols + 1);
}

unsigned TerrainGeometryAssembler::calculateGeometryIndexCount() const
{
    return m_numRows * m_numCols * 6;
}

BoundingVolume TerrainGeometryAssembler::calculateGeometryBounding()
{
    assert((m_numRows != 0) && (m_numCols != 0));
    float min_height = 0.0f;
    float max_height = 1.0f;
    if (m_heightMap)
    {
        const float_buffer heights = m_heightMap.value();
        min_height = heights[0];
        max_height = heights[0];
        for (unsigned vi = 1; vi < m_vtxUsedCount; vi++)
        {
            if (heights[vi] < min_height) min_height = heights[vi];
            if (heights[vi] > max_height) max_height = heights[vi];
        }
    }
    const Vector3 box_corner[2] = { Vector3(m_minPosition.x(), min_height, m_minPosition.z()),
        Vector3(m_maxPosition.x(), max_height, m_maxPosition.z()) };
    const Box3 bb_box = ContainmentBox3::ComputeAlignedBox(box_corner, 2);
    return BoundingVolume{ bb_box };
}

TerrainGeometryDisassembler::TerrainGeometryDisassembler() : TriangleListDisassembler()
{
    m_numRows = m_numCols = 1;
}

void TerrainGeometryDisassembler::disassemble(const Engine::GenericDto& dto)
{
    TriangleListDisassembler::disassemble(dto);
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_NUM_ROWS)) m_numRows = v.value();
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_NUM_COLS)) m_numCols = v.value();
    if (auto v = dto.tryGetValue<Vector3>(TOKEN_MIN_POSITION)) m_minPosition = v.value();
    if (auto v = dto.tryGetValue<Vector3>(TOKEN_MAX_POSITION)) m_maxPosition = v.value();
    if (auto v = dto.tryGetValue<Vector2>(TOKEN_MIN_TEXTURE_COORDINATE)) m_minTextureCoordinate = v.value();
    if (auto v = dto.tryGetValue<Vector2>(TOKEN_MAX_TEXTURE_COORDINATE)) m_maxTextureCoordinate = v.value();
    if (auto v = dto.tryGetValue<float_buffer>(TOKEN_HEIGHT_MAP)) m_heightMap = v.value();
}

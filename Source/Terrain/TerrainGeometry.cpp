#include "TerrainGeometry.h"
#include "TerrainGeometryDto.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Terrain;
using namespace Enigma::Engine;

DEFINE_RTTI(Terrain, TerrainGeometry, TriangleList);

TerrainGeometry::TerrainGeometry(const std::string& name) : TriangleList(name)
{
    m_factoryDesc = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.GetName());
}

TerrainGeometry::TerrainGeometry(const GenericDto& o) : TriangleList(o)
{
    TerrainGeometryDto dto = TerrainGeometryDto::FromGenericDto(o);
    m_numRows = dto.NumRows();
    m_numCols = dto.NumCols();
    m_minPosition = dto.MinPosition();
    m_maxPosition = dto.MaxPosition();
    m_minTextureCoordinate = dto.MinTextureCoordinate();
    m_maxTextureCoordinate = dto.MaxTextureCoordinate();
    if (dto.HeightMap())
    {
        m_heightMap = dto.HeightMap().value();
    }
    if (!dto.Position3s())
    {
        dto.ConvertGeometryVertices();
        GeometryData::DeserializeGeometryDto(dto);
    }
}

TerrainGeometry::~TerrainGeometry()
{
}

GenericDto TerrainGeometry::SerializeDto() const
{
    TerrainGeometryDto dto;
    SerializeNonVertexAttributes(dto);
    dto.Name() = m_name;
    dto.NumRows() = m_numRows;
    dto.NumCols() = m_numCols;
    dto.MinPosition() = m_minPosition;
    dto.MaxPosition() = m_maxPosition;
    dto.MinTextureCoordinate() = m_minTextureCoordinate;
    dto.MaxTextureCoordinate() = m_maxTextureCoordinate;
    if (!m_heightMap.empty())
    {
        dto.HeightMap() = m_heightMap;
    }
    return dto.ToGenericDto();
}

void TerrainGeometry::UpdateHeightMapToVertexMemory()
{
    assert(m_numRows > 0 && m_numCols > 0);
    if (FATAL_LOG_EXPR(m_heightMap.empty())) return;
    //todo : check performance later
    for (unsigned i = 0; i < m_heightMap.size(); i++)
    {
        SetPosition3(i, GetPosition3(i) + MathLib::Vector3(0.0f, m_heightMap[i], 0.0f));
    }
}

void TerrainGeometry::RangedUpdateHeightMapToVertexMemory(unsigned offset, unsigned count)
{
    assert(m_numRows > 0 && m_numCols > 0);
    if (FATAL_LOG_EXPR(m_heightMap.empty())) return;
    //todo : check performance later
    for (unsigned i = offset; i < offset + count; i++)
    {
        SetPosition3(i, GetPosition3(i) + MathLib::Vector3(0.0f, m_heightMap[i], 0.0f));
    }
}

Enigma::MathLib::Dimension<float> TerrainGeometry::GetCellDimension() const
{
    assert(m_numCols > 0 && m_numRows > 0);
    MathLib::Dimension<float> cell_dimension;
    cell_dimension.m_width = (m_maxPosition.X() - m_minPosition.X()) / static_cast<float>(m_numCols);
    cell_dimension.m_height = (m_maxPosition.Z() - m_minPosition.Z()) / static_cast<float>(m_numRows);

    return cell_dimension;
}

void TerrainGeometry::ChangeHeight(unsigned idx, float new_height)
{
    assert(idx < m_heightMap.size());
    m_heightMap[idx] = new_height;
}

#include "TerrainGeometry.h"
#include "TerrainGeometryDto.h"

using namespace Enigma::Terrain;
using namespace Enigma::Engine;

DEFINE_RTTI(Terrain, TerrainGeometry, TriangleList);

TerrainGeometry::TerrainGeometry(const std::string& name) : TriangleList(name)
{

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

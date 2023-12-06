#include "TerrainGeometry.h"
#include "TerrainGeometryDto.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Terrain;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

DEFINE_RTTI(Terrain, TerrainGeometry, TriangleList);

TerrainGeometry::TerrainGeometry(const std::string& name) : TriangleList(name)
{
    m_factoryDesc = Engine::FactoryDesc(TerrainGeometry::TYPE_RTTI.getName());
}

TerrainGeometry::TerrainGeometry(const GenericDto& o) : TriangleList(o)
{
    TerrainGeometryDto dto = TerrainGeometryDto::fromGenericDto(o);
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
    else
    {
        m_heightMap = std::vector<float>((m_numRows + 1) * (m_numCols + 1));
        std::memset(m_heightMap.data(), 0, m_heightMap.size() * sizeof(float));
    }
    if (!dto.Position3s())
    {
        dto.ConvertGeometryVertices();
        GeometryData::deserializeGeometryDto(dto);
    }
}

TerrainGeometry::~TerrainGeometry()
{
}

GenericDto TerrainGeometry::serializeDto() const
{
    TerrainGeometryDto dto;
    serializeNonVertexAttributes(dto);
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
    return dto.toGenericDto();
}

void TerrainGeometry::UpdateHeightMapToVertexMemory()
{
    RangedUpdateHeightMapToVertexMemory(0, m_heightMap.size());
}

void TerrainGeometry::RangedUpdateHeightMapToVertexMemory(unsigned offset, unsigned count)
{
    assert(m_numRows > 0 && m_numCols > 0);
    if (FATAL_LOG_EXPR(m_heightMap.empty())) return;
    //todo : check performance later
    for (unsigned i = offset; i < offset + count; i++)
    {
        auto pos = getPosition3(i);
        pos.Y() = m_heightMap[i];
        setPosition3(i, pos);
    }
}

void TerrainGeometry::UpdateVertexNormals()
{
    RangedUpdateVertexNormals(0, m_heightMap.size());
}

void TerrainGeometry::RangedUpdateVertexNormals(unsigned offset, unsigned count)
{
    auto dimension = GetCellDimension();
    auto [start_x, start_z] = RevertVertexIndex(offset);
    auto [end_x, end_z] = RevertVertexIndex(offset + count - 1);
    if (start_x > 0) start_x--;
    if (start_z > 0) start_z--;
    if (end_x < m_numCols) end_x++;
    if (end_z < m_numRows) end_z++;

    unsigned vtx_num_x = m_numCols + 1;
    // calculate normal
    for (unsigned ix = start_x; ix <= end_x; ix++)
    {
        for (unsigned iz = start_z; iz <= end_z; iz++)
        {
            float h0 = m_heightMap[iz * vtx_num_x + ix];
            float h1 = 0.0f;
            if (ix < m_numCols)
            {
                h1 = m_heightMap[iz * vtx_num_x + ix + 1] - h0;
            }
            float h2 = 0.0f;
            if (iz > 0)
            {
                h2 = m_heightMap[(iz - 1) * vtx_num_x + ix] - h0;
            }
            float h3 = 0.0f;
            if (ix > 0)
            {
                h3 = m_heightMap[iz * vtx_num_x + ix - 1] - h0;
            }
            float h4 = 0.0f;
            if (iz < m_numRows)
            {
                h4 = m_heightMap[(iz + 1) * vtx_num_x + ix] - h0;
            }
            float h5 = 0.0f;
            if ((iz < m_numRows) && (ix < m_numCols))
            {
                h5 = m_heightMap[(iz + 1) * vtx_num_x + ix + 1] - h0;
            }
            float h6 = 0.0f;
            if ((iz > 0) && (ix > 0))
            {
                h6 = m_heightMap[(iz - 1) * vtx_num_x + ix - 1] - h0;
            }
            Vector3 vecEdge1 = Vector3(dimension.m_width, h1, 0.0f).Normalize();
            Vector3 vecEdge2 = Vector3(0.0f, h2, -dimension.m_height).Normalize();
            Vector3 vecEdge3 = Vector3(-dimension.m_width, h3, 0.0f).Normalize();
            Vector3 vecEdge4 = Vector3(0.0f, h4, dimension.m_height).Normalize();
            Vector3 vecEdge5 = Vector3(dimension.m_width, h5, dimension.m_height).Normalize();
            Vector3 vecEdge6 = Vector3(-dimension.m_width, h6, -dimension.m_height).Normalize();
            Vector3 vecFace1 = vecEdge1.Cross(vecEdge2);
            Vector3 vecFace2 = vecEdge2.Cross(vecEdge6);
            Vector3 vecFace3 = vecEdge6.Cross(vecEdge3);
            Vector3 vecFace4 = vecEdge3.Cross(vecEdge4);
            Vector3 vecFace5 = vecEdge4.Cross(vecEdge5);
            Vector3 vecFace6 = vecEdge5.Cross(vecEdge1);
            Vector3 vecNor = vecFace1 + vecFace2 + vecFace3 + vecFace4 + vecFace5 + vecFace6;
            vecNor.NormalizeSelf();

            // vtx idx
            int vtxIdx = iz * vtx_num_x + ix;
            setVertexNormal(vtxIdx, vecNor);
        }
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

float TerrainGeometry::GetHeight(unsigned x, unsigned z) const
{
    unsigned idx = ConvertVertexIndex(x, z);
    assert(idx < m_heightMap.size());
    return m_heightMap[idx];
}

void TerrainGeometry::ChangeHeight(unsigned x, unsigned z, float new_height)
{
    unsigned idx = ConvertVertexIndex(x, z);
    assert(idx < m_heightMap.size());
    m_heightMap[idx] = new_height;
}

std::tuple<unsigned, unsigned> TerrainGeometry::LocateCell(const MathLib::Vector3& position) const
{
    auto dimension = GetCellDimension();
    unsigned cell_x = static_cast<unsigned>(std::floorf((position.X() - m_minPosition.X()) / dimension.m_width + 0.5f));
    unsigned cell_z = static_cast<unsigned>(std::floorf((position.Z() - m_minPosition.Z()) / dimension.m_height + 0.5f));
    return std::make_tuple(cell_x, cell_z);
}

unsigned TerrainGeometry::ConvertVertexIndex(unsigned x, unsigned z) const
{
    assert(m_numCols > 0 && m_numRows > 0);
    assert(x <= m_numCols && z <= m_numRows);
    return x + z * (m_numCols + 1);
}

std::tuple<unsigned, unsigned> TerrainGeometry::RevertVertexIndex(unsigned idx) const
{
    assert(m_numCols > 0 && m_numRows > 0);
    assert(idx < m_heightMap.size());
    unsigned x = idx % (m_numCols + 1);
    unsigned z = idx / (m_numCols + 1);
    return std::make_tuple(x, z);
}

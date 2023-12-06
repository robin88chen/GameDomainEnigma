#include "GeometryData.h"
#include "EngineErrors.h"
#include "Platforms/PlatformLayer.h"
#include "MathLib/ContainmentBox3.h"
#include <cassert>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;
using namespace Enigma::Graphics;
using namespace Enigma::MathLib;

DEFINE_RTTI_OF_BASE(Engine, GeometryData);

GeometryData::GeometryData(const std::string& name) : m_factoryDesc(GeometryData::TYPE_RTTI.getName())
{
    m_name = name;
    m_vtxCapacity = m_idxCapacity = 0;
    m_vtxUsedCount = m_idxUsedCount = 0;
    m_geoSegmentVector.reserve(8);
    m_topology = PrimitiveTopology::Topology_Undefine;

    m_geometryBound = BoundingVolume{Box3::UNIT_BOX};
}

GeometryData::GeometryData(const GenericDto& o) : m_factoryDesc(o.GetRtti())
{
    m_name = o.getName();
    GeometryDataDto dto = GeometryDataDto::fromGenericDto(o);
    deserializeGeometryDto(dto);
}

GeometryData::~GeometryData()
{

}

GenericDto GeometryData::serializeDto() const
{
    return serializeGeometryDto().toGenericDto();
}

GeometryDataDto GeometryData::serializeGeometryDto() const
{
    GeometryDataDto dto;
    serializeNonVertexAttributes(dto);
    if (m_vertexDesc.HasPosition3())
    {
        dto.Position3s() = getPosition3Array(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasPosition4())
    {
        dto.Position4s() = getPosition4Array(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasNormal())
    {
        dto.Normals() = getVertexNormalArray(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasDiffuseColor(VertexDescription::ColorNumeric::Float))
    {
        dto.DiffuseColors() = getDiffuseColorArray(VertexDescription::ColorNumeric::Float, m_vtxUsedCount);
    }
    if (m_vertexDesc.HasSpecularColor(VertexDescription::ColorNumeric::Float))
    {
        dto.SpecularColors() = getSpecularColorArray(VertexDescription::ColorNumeric::Float, m_vtxUsedCount);
    }
    for (unsigned i = 0; i < VertexFormatCode::MAX_TEX_COORD; i++)
    {
        TextureCoordDto texture_coord_dto;
        if (m_vertexDesc.HasTextureCoord(i, 2))
        {
            texture_coord_dto.Texture2DCoords() = getTexture2DCoordArray(i, m_vtxUsedCount);
        }
        else if (m_vertexDesc.HasTextureCoord(i, 1))
        {
            texture_coord_dto.Texture1DCoords() = getTexture1DCoordArray(i, m_vtxUsedCount);
        }
        else if (m_vertexDesc.HasTextureCoord(i, 3))
        {
            texture_coord_dto.Texture3DCoords() = getTexture3DCoordArray(i, m_vtxUsedCount);
        }
        else
        {
            break;
        }
        dto.TextureCoords().push_back(texture_coord_dto.toGenericDto());
    }
    if (m_vertexDesc.HasPaletteIndex())
    {
        dto.PaletteIndices() = getPaletteIndexArray(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasBlendWeight())
    {
        dto.Weights() = getTotalSkinWeightArray(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasTangent())
    {
        dto.Tangents() = getVertexTangentArray(m_vtxUsedCount);
    }
    if (!m_indexMemory.empty())
    {
        dto.Indices() = getIndexMemory();
    }

    return dto;
}

void GeometryData::serializeNonVertexAttributes(GeometryDataDto& dto) const
{
    dto.factoryDesc() = m_factoryDesc;
    dto.Name() = m_name;
    dto.VertexFormat() = m_vertexFormatCode.ToString();
    dto.VertexCapacity() = m_vtxCapacity;
    dto.VertexUsedCount() = m_vtxUsedCount;
    dto.IndexCapacity() = m_idxCapacity;
    dto.IndexUsedCount() = m_idxUsedCount;
    for (unsigned i = 0; i < m_geoSegmentVector.size(); i++)
    {
        dto.Segments().emplace_back(m_geoSegmentVector[i].m_startVtx);
        dto.Segments().emplace_back(m_geoSegmentVector[i].m_vtxCount);
        dto.Segments().emplace_back(m_geoSegmentVector[i].m_startIdx);
        dto.Segments().emplace_back(m_geoSegmentVector[i].m_idxCount);
    }
    dto.Topology() = static_cast<unsigned>(m_topology);
    dto.GeometryBound() = m_geometryBound.serializeDto().toGenericDto();
}

void GeometryData::deserializeGeometryDto(const GeometryDataDto& dto)
{
    createVertexCapacity(dto.VertexFormat(), dto.VertexCapacity(), dto.VertexUsedCount(),
        dto.IndexCapacity(), dto.IndexUsedCount());
    m_geoSegmentVector.clear();
    for (unsigned i = 0; i < dto.Segments().size(); i += 4)
    {
        m_geoSegmentVector.emplace_back(dto.Segments()[i], dto.Segments()[i + 1],
            dto.Segments()[i + 2], dto.Segments()[i + 3]);
    }
    m_topology = static_cast<PrimitiveTopology>(dto.Topology());
    if (auto pos3 = dto.Position3s())
    {
        setPosition3Array(pos3.value());
    }
    if (auto pos4 = dto.Position4s())
    {
        setPosition4Array(pos4.value());
    }
    if (auto nor = dto.Normals())
    {
        setVertexNormalArray(nor.value());
    }
    if (auto diff = dto.DiffuseColors())
    {
        setDiffuseColorArray(diff.value());
    }
    if (auto spe = dto.SpecularColors())
    {
        setSpecularColorArray(spe.value());
    }
    for (unsigned i = 0; i < dto.TextureCoords().size(); i++)
    {
        TextureCoordDto coord = TextureCoordDto::fromGenericDto(dto.TextureCoords()[i]);
        if (auto tex2 = coord.Texture2DCoords())
        {
            setTexture2DCoordArray(i, tex2.value());
        }
        else if (auto tex1 = coord.Texture1DCoords())
        {
            setTexture1DCoordArray(i, tex1.value());
        }
        else if (auto tex3 = coord.Texture3DCoords())
        {
            setTexture3DCoordArray(i, tex3.value());
        }
    }
    if (auto pal = dto.PaletteIndices())
    {
        setPaletteIndexArray(pal.value());
    }
    if (auto w = dto.Weights())
    {
        setTotalSkinWeightArray(w.value());
    }
    if (auto t = dto.Tangents())
    {
        setVertexTangentArray(t.value());
    }
    if (auto idx = dto.Indices())
    {
        setIndexArray(idx.value());
    }
    m_geometryBound = BoundingVolume(BoundingVolumeDto::fromGenericDto(dto.GeometryBound()));
}

RenderBufferSignature GeometryData::makeRenderBufferSignature() const
{
    return RenderBufferSignature{ m_name + ".rndbuf", m_topology, m_vtxCapacity, m_idxCapacity };
}

error GeometryData::createVertexCapacity(const std::string& vertex_format_string, unsigned vtx_capa, unsigned vtx_count, unsigned idx_capa, unsigned idx_count)
{
    m_vertexFormatCode.FromString(vertex_format_string);
    m_vertexDesc = m_vertexFormatCode.CalculateVertexSize();

    if (vtx_capa)
    {
        m_vertexMemory.resize(static_cast<size_t>(vtx_capa) * m_vertexDesc.TotalVertexSize());
    }
    if (idx_capa)
    {
        m_indexMemory.resize(idx_capa);
    }
    m_vtxCapacity = vtx_capa;
    m_idxCapacity = idx_capa;
    m_vtxUsedCount = vtx_count;
    m_idxUsedCount = idx_count;

    m_geoSegmentVector.clear();
    m_geoSegmentVector.push_back(GeometrySegment());
    m_geoSegmentVector[0].m_startVtx = 0;
    m_geoSegmentVector[0].m_vtxCount = m_vtxUsedCount;
    m_geoSegmentVector[0].m_startIdx = 0;
    m_geoSegmentVector[0].m_idxCount = m_idxUsedCount;

    return ErrorCode::ok;
}

error GeometryData::resizeVertexMemoryCapacity(unsigned vtx_capa, unsigned idx_capa)
{
    if (vtx_capa)
    {
        m_vertexMemory.resize(static_cast<size_t>(vtx_capa) * m_vertexDesc.TotalVertexSize());
        m_vtxCapacity = vtx_capa;
    }
    if (idx_capa)
    {
        m_indexMemory.resize(idx_capa);
        m_idxCapacity = idx_capa;
    }

    return ErrorCode::ok;
}

void GeometryData::setUsingVertexCount(unsigned vtx_count, unsigned idx_count)
{
    if (vtx_count <= m_vtxCapacity)
    {
        m_vtxUsedCount = vtx_count;
    }
    if (idx_count <= m_idxCapacity)
    {
        m_idxUsedCount = idx_count;
    }
    if (m_geoSegmentVector.size() == 1)
    {
        m_geoSegmentVector[0].m_startVtx = 0;
        m_geoSegmentVector[0].m_vtxCount = m_vtxUsedCount;
        m_geoSegmentVector[0].m_startIdx = 0;
        m_geoSegmentVector[0].m_idxCount = m_idxUsedCount;
    }
}

Vector3 GeometryData::getPosition3(unsigned vtxIndex)
{
    Vector3 pos;
    getVertexMemoryData(vtxIndex, m_vertexDesc.PositionOffset(), m_vertexDesc.PositionDimension(), 3, (float*)pos, true);
    return pos;
}

Vector4 GeometryData::getPosition4(unsigned vtxIndex)
{
    Vector4 pos;
    getVertexMemoryData(vtxIndex, m_vertexDesc.PositionOffset(), m_vertexDesc.PositionDimension(), 4, (float*)pos, true);
    return pos;
}

std::vector<Vector3> GeometryData::getPosition3Array(unsigned count) const
{
    return getPosition3Array(0, count);
}

std::vector<Vector3> GeometryData::getPosition3Array(unsigned offset, unsigned count) const
{
    std::vector<Vector3> positions;
    positions.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 3, reinterpret_cast<float*>(&positions[0]), count, true);
    return positions;
}

std::vector<Vector4> GeometryData::getPosition4Array(unsigned count) const
{
    return getPosition4Array(0, count);
}

std::vector<Vector4> GeometryData::getPosition4Array(unsigned offset, unsigned count) const
{
    std::vector<Vector4> positions;
    positions.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 4, reinterpret_cast<float*>(&positions[0]), count, true);
    return positions;
}

error GeometryData::setPosition3(unsigned vtxIndex, const MathLib::Vector3& position)
{
    return setVertexMemoryData(vtxIndex, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 3, (const float*)position, true);
}

error GeometryData::setPosition4(unsigned vtxIndex, const MathLib::Vector4& position)
{
    return setVertexMemoryData(vtxIndex, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 4, (const float*)position, true);
}

error GeometryData::setPosition3Array(const std::vector<MathLib::Vector3>& positions)
{
    return setPosition3Array(0, positions);
}

error GeometryData::setPosition3Array(unsigned offset, const std::vector<MathLib::Vector3>& positions)
{
    return setVertexMemoryDataArray(offset, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 3, reinterpret_cast<const float*>(&positions[0]), static_cast<unsigned>(positions.size()), true);
}

error GeometryData::setPosition4Array(const std::vector<MathLib::Vector4>& positions)
{
    return setPosition4Array(0, positions);
}

error GeometryData::setPosition4Array(unsigned offset, const std::vector<MathLib::Vector4>& positions)
{
    return setVertexMemoryDataArray(offset, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 4, reinterpret_cast<const float*>(&positions[0]), static_cast<unsigned>(positions.size()), true);
}

Vector3 GeometryData::getVertexNormal(unsigned vtxIndex) const
{
    Vector3 nor;
    getVertexMemoryData(vtxIndex, m_vertexDesc.NormalOffset(), 3, 3, (float*)nor, false);
    return nor;
}

error GeometryData::setVertexNormal(unsigned vtxIndex, const MathLib::Vector3& nor)
{
    return setVertexMemoryData(vtxIndex, m_vertexDesc.NormalOffset(), 3, 3, (const float*)nor, false);
}

std::vector<Vector3> GeometryData::getVertexNormalArray(unsigned count) const
{
    return getVertexNormalArray(0, count);
}

std::vector<Vector3> GeometryData::getVertexNormalArray(unsigned offset, unsigned count) const
{
    std::vector<Vector3> normals;
    normals.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.NormalOffset(), 3, 3, reinterpret_cast<float*>(&normals[0]), count, false);
    return normals;
}

error GeometryData::setVertexNormalArray(const std::vector<MathLib::Vector3>& normals)
{
    return setVertexNormalArray(0, normals);
}

error GeometryData::setVertexNormalArray(unsigned offset, const std::vector<MathLib::Vector3>& normals)
{
    return setVertexMemoryDataArray(offset, m_vertexDesc.NormalOffset(), 3, 3, reinterpret_cast<const float*>(&normals[0]),
        static_cast<unsigned>(normals.size()), false);
}

error GeometryData::setDiffuseColorArray(const std::vector<MathLib::Vector4>& colors)
{
    return setDiffuseColorArray(0, colors);
}

error GeometryData::setDiffuseColorArray(unsigned offset, const std::vector<MathLib::Vector4>& colors)
{
    return setVertexMemoryDataArray(offset, m_vertexDesc.DiffuseColorOffset(VertexDescription::ColorNumeric::Float),
        m_vertexDesc.DiffuseColorDimension(), 4, reinterpret_cast<const float*>(&colors[0]), static_cast<unsigned>(colors.size()), false);
}

error GeometryData::setDiffuseColorArray(const std::vector<MathLib::ColorRGBA>& colors)
{
    return setDiffuseColorArray(0, colors);
}

error GeometryData::setDiffuseColorArray(unsigned offset, const std::vector<MathLib::ColorRGBA>& colors)
{
    return setVertexMemoryDataArray(offset, m_vertexDesc.DiffuseColorOffset(VertexDescription::ColorNumeric::Float),
        m_vertexDesc.DiffuseColorDimension(), 4, reinterpret_cast<const float*>(&colors[0]), static_cast<unsigned>(colors.size()), false);
}

std::vector<Vector4> GeometryData::getDiffuseColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned count) const
{
    return getDiffuseColorArray(type, 0, count);
}

std::vector<Vector4> GeometryData::getDiffuseColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned offset, unsigned count) const
{
    std::vector<Vector4> colors;
    colors.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.DiffuseColorOffset(type),
        m_vertexDesc.DiffuseColorDimension(), 4, reinterpret_cast<float*>(&colors[0]), count, true);
    return colors;
}

error GeometryData::setSpecularColorArray(const std::vector<MathLib::Vector4>& colors)
{
    return setSpecularColorArray(0, colors);
}

error GeometryData::setSpecularColorArray(unsigned offset, const std::vector<MathLib::Vector4>& colors)
{
    return setVertexMemoryDataArray(offset, m_vertexDesc.SpecularColorOffset(VertexDescription::ColorNumeric::Float),
        m_vertexDesc.SpecularColorDimension(), 4, reinterpret_cast<const float*>(&colors[0]), static_cast<unsigned>(colors.size()), false);
}

std::vector<Vector4> GeometryData::getSpecularColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned count) const
{
    return getSpecularColorArray(type, 0, count);
}

std::vector<Vector4> GeometryData::getSpecularColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned offset, unsigned count) const
{
    std::vector<Vector4> colors;
    colors.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.SpecularColorOffset(type),
        m_vertexDesc.SpecularColorDimension(), 4, reinterpret_cast<float*>(&colors[0]), count, true);
    return colors;
}

std::vector<Vector2> GeometryData::getTexture2DCoordArray(unsigned offset, unsigned stage, unsigned count) const
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    std::vector<Vector2> uvs;
    uvs.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.TextureCoordOffset(stage),
        m_vertexDesc.TextureCoordSize(stage), 2, reinterpret_cast<float*>(&uvs[0]), count, false);
    return uvs;
}

std::vector<Vector2> GeometryData::getTexture2DCoordArray(unsigned stage, unsigned count) const
{
    return getTexture2DCoordArray(0, stage, count);
}

error GeometryData::setTexture2DCoordArray(unsigned offset, unsigned stage, const std::vector<MathLib::Vector2>& uvs)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return setVertexMemoryDataArray(offset, m_vertexDesc.TextureCoordOffset(stage),
        m_vertexDesc.TextureCoordSize(stage), 2, reinterpret_cast<const float*>(&uvs[0]), static_cast<unsigned>(uvs.size()), false);
}

error GeometryData::setTexture2DCoordArray(unsigned stage, const std::vector<MathLib::Vector2>& uvs)
{
    return setTexture2DCoordArray(0, stage, uvs);
}

error GeometryData::setTexture1DCoordArray(unsigned stage, const std::vector<float>& us)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return setVertexMemoryDataArray(0, m_vertexDesc.TextureCoordOffset(stage), m_vertexDesc.TextureCoordSize(stage), 1,
        (const float*)(&us[0]), static_cast<unsigned>(us.size()), false);
}

std::vector<float> GeometryData::getTexture1DCoordArray(unsigned offset, unsigned stage, unsigned count) const
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    std::vector<float> us;
    us.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.TextureCoordOffset(stage),
        m_vertexDesc.TextureCoordSize(stage), 1, &us[0], count, false);
    return us;
}

std::vector<float> GeometryData::getTexture1DCoordArray(unsigned stage, unsigned count) const
{
    return getTexture1DCoordArray(0, stage, count);
}

error GeometryData::setTexture3DCoordArray(unsigned stage, const std::vector<MathLib::Vector3>& uvws)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return setVertexMemoryDataArray(0, m_vertexDesc.TextureCoordOffset(stage), m_vertexDesc.TextureCoordSize(stage), 3,
        reinterpret_cast<const float*>(&uvws[0]), static_cast<unsigned>(uvws.size()), false);
}

std::vector<Vector3> GeometryData::getTexture3DCoordArray(unsigned stage, unsigned count) const
{
    return getTexture3DCoordArray(0, stage, count);
}

std::vector<Vector3> GeometryData::getTexture3DCoordArray(unsigned offset, unsigned stage, unsigned count) const
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    std::vector<Vector3> uvws;
    uvws.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.TextureCoordOffset(stage),
        m_vertexDesc.TextureCoordSize(stage), 3, reinterpret_cast<float*>(&uvws[0]), count, false);
    return uvws;
}

error GeometryData::setPaletteIndexArray(const std::vector<unsigned>& palette_array)
{
    return setVertexMemoryDataArray(0, m_vertexDesc.PaletteIndexOffset(), 1, 1,
        reinterpret_cast<const float*>(&palette_array[0]), static_cast<unsigned>(palette_array.size()), false);
}

std::vector<unsigned> GeometryData::getPaletteIndexArray(unsigned count) const
{
    return getPaletteIndexArray(0, count);
}

std::vector<unsigned> GeometryData::getPaletteIndexArray(unsigned offset, unsigned count) const
{
    std::vector<unsigned> indices;
    indices.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.PaletteIndexOffset(),
        1, 1, reinterpret_cast<float*>(&indices[0]), count, false);
    return indices;
}

error GeometryData::setSkinWeightArray(unsigned weight_idx, const std::vector<float>& weight_array)
{
    return setVertexMemoryDataArray(0, m_vertexDesc.WeightOffset(weight_idx), 1, 1,
        &weight_array[0], static_cast<unsigned>(weight_array.size()), false);
}

error GeometryData::setTotalSkinWeightArray(const std::vector<float>& weight_array)
{
    if (m_vertexDesc.BlendWeightCount() > 0)
    {
        return setVertexMemoryDataArray(0, m_vertexDesc.WeightOffset(), m_vertexDesc.BlendWeightCount(),
            m_vertexDesc.BlendWeightCount(), &weight_array[0],
            static_cast<unsigned>(weight_array.size()) / m_vertexDesc.BlendWeightCount(), false);
    }
    return ErrorCode::ok;
}

std::vector<float> GeometryData::getTotalSkinWeightArray(unsigned vtx_count) const
{
    return getTotalSkinWeightArray(0, vtx_count);
}

std::vector<float> GeometryData::getTotalSkinWeightArray(unsigned offset, unsigned vtx_count) const
{
    std::vector<float> weights;
    weights.resize(static_cast<size_t>(vtx_count) * m_vertexDesc.BlendWeightCount());
    getVertexMemoryDataArray(offset, m_vertexDesc.WeightOffset(),
        m_vertexDesc.BlendWeightCount(), m_vertexDesc.BlendWeightCount(), &weights[0], vtx_count, false);
    return weights;
}

error GeometryData::setVertexTangentArray(const std::vector<MathLib::Vector4>& tangents)
{
    return setVertexMemoryDataArray(0, m_vertexDesc.TangentOffset(), m_vertexDesc.TangentDimension(), 4,
        reinterpret_cast<const float*>(&tangents[0]), static_cast<unsigned>(tangents.size()), false);
}

std::vector<Vector4> GeometryData::getVertexTangentArray(unsigned count) const
{
    return getVertexTangentArray(0, count);
}

std::vector<Vector4> GeometryData::getVertexTangentArray(unsigned offset, unsigned count) const
{
    std::vector<Vector4> tangents;
    tangents.resize(count);
    getVertexMemoryDataArray(offset, m_vertexDesc.TangentOffset(),
        m_vertexDesc.TangentDimension(), 4, reinterpret_cast<float*>(&tangents[0]), count, true);
    return tangents;
}

error GeometryData::setIndexArray(const std::vector<unsigned>& idx_ary)
{
    assert(idx_ary.size() > 0);

    // 要設index array, 必須有index memory
    if (FATAL_LOG_EXPR(m_indexMemory.size() == 0)) return ErrorCode::nullMemoryBuffer;
    unsigned int data_count = m_idxUsedCount;
    if (idx_ary.size() < data_count) data_count = static_cast<unsigned>(idx_ary.size());
    memcpy(&m_indexMemory[0], &idx_ary[0], data_count * sizeof(unsigned int));
    return ErrorCode::ok;
}

IVertexBuffer::ranged_buffer GeometryData::getRangedVertexMemory(unsigned offset, unsigned count) const
{
    unsigned int byte_offset = offset * m_vertexDesc.TotalVertexSize();
    unsigned int byte_count = count * m_vertexDesc.TotalVertexSize();
    byte_buffer buffer;
    buffer.resize(byte_count);
    memcpy(&buffer[0], &m_vertexMemory[byte_offset], byte_count);
    return { offset, count, buffer };
}

IIndexBuffer::ranged_buffer GeometryData::getRangedIndexMemory(unsigned offset, unsigned count) const
{
    uint_buffer buffer;
    buffer.resize(count);
    memcpy(&buffer[0], &m_indexMemory[offset], count * sizeof(unsigned));
    return { offset, count, buffer };
}

error GeometryData::getVertexMemoryData(unsigned vtxIndex, int elementOffset, int elementDimension,
    int destDimension, float* dest, bool isPos) const
{
    assert(dest);

    if (FATAL_LOG_EXPR(vtxIndex >= m_vtxUsedCount)) return ErrorCode::invalidArrayIndex;

    if (FATAL_LOG_EXPR(m_vertexMemory.size() == 0)) return ErrorCode::nullMemoryBuffer;
    if (FATAL_LOG_EXPR(m_vertexDesc.TotalVertexSize() == 0)) return ErrorCode::zeroVertexSize;

    if (elementOffset < 0) return ErrorCode::ok;  // no need set

    unsigned int step = m_vertexDesc.TotalVertexSize() / sizeof(float);
    unsigned int base_idx = step * vtxIndex + elementOffset;

    //todo: 這邊參數的運算要看怎麼調整
    int cp_dimension = destDimension;
    if (cp_dimension > elementDimension) cp_dimension = elementDimension;
    memcpy(dest, &m_vertexMemory[base_idx * sizeof(float)], cp_dimension * sizeof(float));
    if (isPos)
    {
        if ((destDimension == 4) && (elementDimension == 3))
        {
            dest[3] = 1.0f;
        }
    }

    return ErrorCode::ok;
}

error GeometryData::setVertexMemoryData(unsigned vtxIndex, int elementOffset, int elementDimension,
    int srcDimension, const float* src, bool isPos)
{
    assert(src);

    if (FATAL_LOG_EXPR(vtxIndex >= m_vtxUsedCount)) return ErrorCode::invalidArrayIndex;

    if (FATAL_LOG_EXPR(m_vertexMemory.size() == 0)) return ErrorCode::nullMemoryBuffer;
    if (FATAL_LOG_EXPR(m_vertexDesc.TotalVertexSize() == 0)) return ErrorCode::zeroVertexSize;

    if (elementOffset < 0) return ErrorCode::ok;  // no need set

    unsigned int step = m_vertexDesc.TotalVertexSize() / sizeof(float);
    unsigned int base_idx = step * vtxIndex + elementOffset;
    int cp_dimension = srcDimension;
    if (cp_dimension > elementDimension) cp_dimension = elementDimension;
    memcpy(&m_vertexMemory[base_idx * sizeof(float)], src, cp_dimension * sizeof(float));
    if (isPos)
    {
        if ((srcDimension == 3) && (elementDimension == 4))
        {
            float* mem = reinterpret_cast<float*>(&m_vertexMemory[(base_idx + 3) * sizeof(float)]);
            *mem = 1.0f;
        }
    }

    return ErrorCode::ok;
}

error GeometryData::getVertexMemoryDataArray(unsigned start, int elementOffset, int elementDimension,
    int destDimension, float* dest, unsigned count, bool isPos) const
{
    assert(dest);
    assert(count > 0);

    if (FATAL_LOG_EXPR(m_vertexMemory.size() == 0)) return ErrorCode::nullMemoryBuffer;
    if (FATAL_LOG_EXPR(m_vertexDesc.TotalVertexSize() == 0)) return ErrorCode::zeroVertexSize;

    if (elementOffset < 0)
    {
        memset(dest, 0, static_cast<size_t>(count) * destDimension * sizeof(float));
        return ErrorCode::ok;
    }

    unsigned int step = m_vertexDesc.TotalVertexSize() / sizeof(float);
    unsigned int pos_count = m_vtxUsedCount - start;
    if (count < pos_count) pos_count = count;
    const float* src = reinterpret_cast<const float*>(&m_vertexMemory[(elementOffset + step * start) * sizeof(float)]);
    int cp_dimension = destDimension;
    if (cp_dimension > elementDimension) cp_dimension = elementDimension;
    bool set_vec4 = false;
    if ((isPos) && (destDimension == 4) && (elementDimension == 3)) set_vec4 = true;
    for (unsigned int i = 0; i < pos_count; i++)
    {
        memcpy(&dest[static_cast<size_t>(i) * destDimension], src, cp_dimension * sizeof(float));
        if (set_vec4) dest[i * destDimension + 3] = 1.0f;
        src += step;
    }

    return ErrorCode::ok;
}

error GeometryData::setVertexMemoryDataArray(unsigned start, int elementOffset, int elementDimension,
    int srcDimension, const float* src, unsigned count, bool isPos)
{
    assert(src);
    assert(count > 0);

    if (FATAL_LOG_EXPR(m_vertexMemory.size() == 0)) return ErrorCode::nullMemoryBuffer;
    if (FATAL_LOG_EXPR(m_vertexDesc.TotalVertexSize() == 0)) return ErrorCode::zeroVertexSize;

    if (elementOffset < 0) return ErrorCode::ok;  // no need set

    unsigned int step = m_vertexDesc.TotalVertexSize() / sizeof(float);
    unsigned int pos_count = m_vtxUsedCount - start;
    if (count < pos_count) pos_count = count;
    float* dst = reinterpret_cast<float*>(&m_vertexMemory[(elementOffset + step * start) * sizeof(float)]);
    int cp_dimension = srcDimension;
    if (cp_dimension > elementDimension) cp_dimension = elementDimension;
    bool get_vec4 = false;
    if ((isPos) && (srcDimension == 3) && (elementDimension == 4)) get_vec4 = true;
    for (unsigned int i = 0; i < pos_count; i++)
    {
        memcpy(dst, &src[static_cast<size_t>(i) * srcDimension], cp_dimension * sizeof(float));
        if (get_vec4) dst[3] = 1.0f;
        dst += step;
    }

    return ErrorCode::ok;
}

const GeometrySegment& GeometryData::getSegment(unsigned index) const
{
    assert(index < m_geoSegmentVector.size());
    return m_geoSegmentVector[index];
}

void GeometryData::changeSegment(unsigned index, unsigned start_vtx, unsigned vtx_count, unsigned start_idx, unsigned idx_count)
{
    if (FATAL_LOG_EXPR(index >= m_geoSegmentVector.size())) return;
    m_geoSegmentVector[index].m_startVtx = start_vtx;
    m_geoSegmentVector[index].m_vtxCount = vtx_count;
    m_geoSegmentVector[index].m_startIdx = start_idx;
    m_geoSegmentVector[index].m_idxCount = idx_count;
}

void GeometryData::calculateBoundingVolume(bool axis_align)
{
    if (FATAL_LOG_EXPR(m_vertexMemory.size() == 0)) return;

    if (axis_align)
    {
        m_geometryBound = BoundingVolume(ContainmentBox3::ComputeAlignedBox(
            reinterpret_cast<float*>(&m_vertexMemory[0]), sizeofVertex() / sizeof(float),
            getUsedVertexCount()));
    }
    else
    {
        m_geometryBound = BoundingVolume(ContainmentBox3::ComputeOrientedBox(
            reinterpret_cast<float*>(&m_vertexMemory[0]), sizeofVertex() / sizeof(float),
            getUsedVertexCount()));
    }
}

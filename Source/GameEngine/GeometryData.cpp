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

GeometryData::GeometryData(const std::string& name) : m_factoryDesc(GeometryData::TYPE_RTTI.GetName())
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
    m_name = o.GetName();
    GeometryDataDto dto = GeometryDataDto::FromGenericDto(o);
    CreateVertexCapacity(dto.VertexFormat(), dto.VertexCapacity(), dto.VertexUsedCount(),
                         dto.IndexCapacity(), dto.IndexUsedCount());
    for (unsigned i = 0; i < dto.Segments().size(); i += 4)
    {
        m_geoSegmentVector.emplace_back(dto.Segments()[i], dto.Segments()[i + 1],
            dto.Segments()[i + 2], dto.Segments()[i + 3]);
    }
    m_topology = static_cast<PrimitiveTopology>(dto.Topology());
    if (auto pos3 = dto.Position3s())
    {
        SetPosition3Array(pos3.value());
    }
    if (auto pos4 = dto.Position4s())
    {
        SetPosition4Array(pos4.value());
    }
    if (auto nor = dto.Normals())
    {
        SetVertexNormalArray(nor.value());
    }
    if (auto diff = dto.DiffuseColors())
    {
        SetDiffuseColorArray(diff.value());
    }
    if (auto spe = dto.SpecularColors())
    {
        SetSpecularColorArray(spe.value());
    }
    for (unsigned i = 0; i < dto.TextureCoords().size(); i++)
    {
        TextureCoordDto coord = TextureCoordDto::FromGenericDto(dto.TextureCoords()[i]);
        if (auto tex2 = coord.Texture2DCoords())
        {
            SetTexture2DCoordArray(i, tex2.value());
        }
        else if (auto tex1 = coord.Texture1DCoords())
        {
            SetTexture1DCoordArray(i, tex1.value());
        }
        else if (auto tex3 = coord.Texture3DCoords())
        {
            SetTexture3DCoordArray(i, tex3.value());
        }
    }
    if (auto pal = dto.PaletteIndices())
    {
        SetPaletteIndexArray(pal.value());
    }
    if (auto w = dto.Weights())
    {
        SetTotalSkinWeightArray(w.value());
    }
    if (auto t = dto.Tangents())
    {
        SetVertexTangentArray(t.value());
    }
    if (auto idx = dto.Indices())
    {
        SetIndexArray(idx.value());
    }
    m_geometryBound = BoundingVolume(BoundingVolumeDto::FromGenericDto(dto.GeometryBound()));
}

GeometryData::~GeometryData()
{

}

GenericDto GeometryData::SerializeDto()
{
    return SerializeGeometryDto().ToGenericDto();
}

GeometryDataDto GeometryData::SerializeGeometryDto()
{
    GeometryDataDto dto;
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
    if (m_vertexDesc.HasPosition3())
    {
        dto.Position3s() = GetPosition3Array(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasPosition4())
    {
        dto.Position4s() = GetPosition4Array(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasNormal())
    {
        dto.Normals() = GetVertexNormalArray(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasDiffuseColor(VertexDescription::ColorNumeric::Float))
    {
        dto.DiffuseColors() = GetDiffuseColorArray(VertexDescription::ColorNumeric::Float, m_vtxUsedCount);
    }
    if (m_vertexDesc.HasSpecularColor(VertexDescription::ColorNumeric::Float))
    {
        dto.SpecularColors() = GetSpecularColorArray(VertexDescription::ColorNumeric::Float, m_vtxUsedCount);
    }
    for (unsigned i = 0; i < VertexFormatCode::MAX_TEX_COORD; i++)
    {
        TextureCoordDto texture_coord_dto;
        if (m_vertexDesc.HasTextureCoord(i, 2))
        {
            texture_coord_dto.Texture2DCoords() = GetTexture2DCoordArray(i, m_vtxUsedCount);
        }
        else if (m_vertexDesc.HasTextureCoord(i, 1))
        {
            texture_coord_dto.Texture1DCoords() = GetTexture1DCoordArray(i, m_vtxUsedCount);
        }
        else if (m_vertexDesc.HasTextureCoord(i, 3))
        {
            texture_coord_dto.Texture3DCoords() = GetTexture3DCoordArray(i, m_vtxUsedCount);
        }
        else
        {
            break;
        }
        dto.TextureCoords().push_back(texture_coord_dto.ToGenericDto());
    }
    if (m_vertexDesc.HasPaletteIndex())
    {
        dto.PaletteIndices() = GetPaletteIndexArray(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasBlendWeight())
    {
        dto.Weights() = GetTotalSkinWeightArray(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasTangent())
    {
        dto.Tangents() = GetVertexTangentArray(m_vtxUsedCount);
    }
    if (!m_indexMemory.empty())
    {
        dto.Indices() = GetIndexMemory();
    }
    dto.GeometryBound() = m_geometryBound.SerializeDto().ToGenericDto();

    return dto;
}

RenderBufferSignature GeometryData::MakeRenderBufferSignature() const
{
    return RenderBufferSignature{ m_name + ".rndbuf", m_topology, m_vtxCapacity, m_idxCapacity };
}

error GeometryData::CreateVertexCapacity(const std::string& vertex_format_string, unsigned vtx_capa, unsigned vtx_count, unsigned idx_capa, unsigned idx_count)
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

    m_geoSegmentVector.push_back(GeometrySegment());
    m_geoSegmentVector[0].m_startVtx = 0;
    m_geoSegmentVector[0].m_vtxCount = m_vtxUsedCount;
    m_geoSegmentVector[0].m_startIdx = 0;
    m_geoSegmentVector[0].m_idxCount = m_idxUsedCount;

    return ErrorCode::ok;
}

error GeometryData::ReSizeVertexMemoryCapacity(unsigned vtx_capa, unsigned idx_capa)
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

void GeometryData::SetUsingVertexCount(unsigned vtx_count, unsigned idx_count)
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

Vector3 GeometryData::GetPosition3(unsigned vtxIndex)
{
    Vector3 pos;
    GetVertexMemoryData(vtxIndex, m_vertexDesc.PositionOffset(), m_vertexDesc.PositionDimension(), 3, (float*)pos, true);
    return pos;
}

Vector4 GeometryData::GetPosition4(unsigned vtxIndex)
{
    Vector4 pos;
    GetVertexMemoryData(vtxIndex, m_vertexDesc.PositionOffset(), m_vertexDesc.PositionDimension(), 4, (float*)pos, true);
    return pos;
}

std::vector<Vector3> GeometryData::GetPosition3Array(unsigned count)
{
    return GetPosition3Array(0, count);
}

std::vector<Vector3> GeometryData::GetPosition3Array(unsigned offset, unsigned count)
{
    std::vector<Vector3> positions;
    positions.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 3, reinterpret_cast<float*>(&positions[0]), count, true);
    return positions;
}

std::vector<Vector4> GeometryData::GetPosition4Array(unsigned count)
{
    return GetPosition4Array(0, count);
}

std::vector<Vector4> GeometryData::GetPosition4Array(unsigned offset, unsigned count)
{
    std::vector<Vector4> positions;
    positions.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 4, reinterpret_cast<float*>(&positions[0]), count, true);
    return positions;
}

error GeometryData::SetPosition3(unsigned vtxIndex, const MathLib::Vector3& position)
{
    return SetVertexMemoryData(vtxIndex, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 3, (const float*)position, true);
}

error GeometryData::SetPosition4(unsigned vtxIndex, const MathLib::Vector4& position)
{
    return SetVertexMemoryData(vtxIndex, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 4, (const float*)position, true);
}

error GeometryData::SetPosition3Array(const std::vector<MathLib::Vector3>& positions)
{
    return SetPosition3Array(0, positions);
}

error GeometryData::SetPosition3Array(unsigned offset, const std::vector<MathLib::Vector3>& positions)
{
    return SetVertexMemoryDataArray(offset, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 3, reinterpret_cast<const float*>(&positions[0]), static_cast<unsigned>(positions.size()), true);
}

error GeometryData::SetPosition4Array(const std::vector<MathLib::Vector4>& positions)
{
    return SetPosition4Array(0, positions);
}

error GeometryData::SetPosition4Array(unsigned offset, const std::vector<MathLib::Vector4>& positions)
{
    return SetVertexMemoryDataArray(offset, m_vertexDesc.PositionOffset(),
        m_vertexDesc.PositionDimension(), 4, reinterpret_cast<const float*>(&positions[0]), static_cast<unsigned>(positions.size()), true);
}

Vector3 GeometryData::GetVertexNormal(unsigned vtxIndex)
{
    Vector3 nor;
    GetVertexMemoryData(vtxIndex, m_vertexDesc.NormalOffset(), 3, 3, (float*)nor, false);
    return nor;
}

error GeometryData::SetVertexNormal(unsigned vtxIndex, const MathLib::Vector3& nor)
{
    return SetVertexMemoryData(vtxIndex, m_vertexDesc.NormalOffset(), 3, 3, (const float*)nor, false);
}

std::vector<Vector3> GeometryData::GetVertexNormalArray(unsigned count)
{
    return GetVertexNormalArray(0, count);
}

std::vector<Vector3> GeometryData::GetVertexNormalArray(unsigned offset, unsigned count)
{
    std::vector<Vector3> normals;
    normals.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.NormalOffset(), 3, 3, reinterpret_cast<float*>(&normals[0]), count, false);
    return normals;
}

error GeometryData::SetVertexNormalArray(const std::vector<MathLib::Vector3>& normals)
{
    return SetVertexNormalArray(0, normals);
}

error GeometryData::SetVertexNormalArray(unsigned offset, const std::vector<MathLib::Vector3>& normals)
{
    return SetVertexMemoryDataArray(offset, m_vertexDesc.NormalOffset(), 3, 3, reinterpret_cast<const float*>(&normals[0]),
        static_cast<unsigned>(normals.size()), false);
}

error GeometryData::SetDiffuseColorArray(const std::vector<MathLib::Vector4>& colors)
{
    return SetDiffuseColorArray(0, colors);
}

error GeometryData::SetDiffuseColorArray(unsigned offset, const std::vector<MathLib::Vector4>& colors)
{
    return SetVertexMemoryDataArray(offset, m_vertexDesc.DiffuseColorOffset(VertexDescription::ColorNumeric::Float),
        m_vertexDesc.DiffuseColorDimension(), 4, reinterpret_cast<const float*>(&colors[0]), static_cast<unsigned>(colors.size()), false);
}

error GeometryData::SetDiffuseColorArray(const std::vector<MathLib::ColorRGBA>& colors)
{
    return SetDiffuseColorArray(0, colors);
}

error GeometryData::SetDiffuseColorArray(unsigned offset, const std::vector<MathLib::ColorRGBA>& colors)
{
    return SetVertexMemoryDataArray(offset, m_vertexDesc.DiffuseColorOffset(VertexDescription::ColorNumeric::Float),
        m_vertexDesc.DiffuseColorDimension(), 4, reinterpret_cast<const float*>(&colors[0]), static_cast<unsigned>(colors.size()), false);
}

std::vector<Vector4> GeometryData::GetDiffuseColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned count)
{
    return GetDiffuseColorArray(type, 0, count);
}

std::vector<Vector4> GeometryData::GetDiffuseColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned offset, unsigned count)
{
    std::vector<Vector4> colors;
    colors.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.DiffuseColorOffset(type),
        m_vertexDesc.DiffuseColorDimension(), 4, reinterpret_cast<float*>(&colors[0]), count, true);
    return colors;
}

error GeometryData::SetSpecularColorArray(const std::vector<MathLib::Vector4>& colors)
{
    return SetSpecularColorArray(0, colors);
}

error GeometryData::SetSpecularColorArray(unsigned offset, const std::vector<MathLib::Vector4>& colors)
{
    return SetVertexMemoryDataArray(offset, m_vertexDesc.SpecularColorOffset(VertexDescription::ColorNumeric::Float),
        m_vertexDesc.SpecularColorDimension(), 4, reinterpret_cast<const float*>(&colors[0]), static_cast<unsigned>(colors.size()), false);
}

std::vector<Vector4> GeometryData::GetSpecularColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned count)
{
    return GetSpecularColorArray(type, 0, count);
}

std::vector<Vector4> GeometryData::GetSpecularColorArray(Graphics::VertexDescription::ColorNumeric type, unsigned offset, unsigned count)
{
    std::vector<Vector4> colors;
    colors.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.SpecularColorOffset(type),
        m_vertexDesc.SpecularColorDimension(), 4, reinterpret_cast<float*>(&colors[0]), count, true);
    return colors;
}

std::vector<Vector2> GeometryData::GetTexture2DCoordArray(unsigned offset, unsigned stage, unsigned count)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    std::vector<Vector2> uvs;
    uvs.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.TextureCoordOffset(stage),
        m_vertexDesc.TextureCoordSize(stage), 2, reinterpret_cast<float*>(&uvs[0]), count, false);
    return uvs;
}

std::vector<Vector2> GeometryData::GetTexture2DCoordArray(unsigned stage, unsigned count)
{
    return GetTexture2DCoordArray(0, stage, count);
}

error GeometryData::SetTexture2DCoordArray(unsigned offset, unsigned stage, const std::vector<MathLib::Vector2>& uvs)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return SetVertexMemoryDataArray(offset, m_vertexDesc.TextureCoordOffset(stage),
        m_vertexDesc.TextureCoordSize(stage), 2, reinterpret_cast<const float*>(&uvs[0]), static_cast<unsigned>(uvs.size()), false);
}

error GeometryData::SetTexture2DCoordArray(unsigned stage, const std::vector<MathLib::Vector2>& uvs)
{
    return SetTexture2DCoordArray(0, stage, uvs);
}

error GeometryData::SetTexture1DCoordArray(unsigned stage, const std::vector<float>& us)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return SetVertexMemoryDataArray(0, m_vertexDesc.TextureCoordOffset(stage), m_vertexDesc.TextureCoordSize(stage), 1,
        (const float*)(&us[0]), static_cast<unsigned>(us.size()), false);
}

std::vector<float> GeometryData::GetTexture1DCoordArray(unsigned offset, unsigned stage, unsigned count)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    std::vector<float> us;
    us.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.TextureCoordOffset(stage),
        m_vertexDesc.TextureCoordSize(stage), 1, &us[0], count, false);
    return us;
}

std::vector<float> GeometryData::GetTexture1DCoordArray(unsigned stage, unsigned count)
{
    return GetTexture1DCoordArray(0, stage, count);
}

error GeometryData::SetTexture3DCoordArray(unsigned stage, const std::vector<MathLib::Vector3>& uvws)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return SetVertexMemoryDataArray(0, m_vertexDesc.TextureCoordOffset(stage), m_vertexDesc.TextureCoordSize(stage), 3,
        reinterpret_cast<const float*>(&uvws[0]), static_cast<unsigned>(uvws.size()), false);
}

std::vector<Vector3> GeometryData::GetTexture3DCoordArray(unsigned stage, unsigned count)
{
    return GetTexture3DCoordArray(0, stage, count);
}

std::vector<Vector3> GeometryData::GetTexture3DCoordArray(unsigned offset, unsigned stage, unsigned count)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    std::vector<Vector3> uvws;
    uvws.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.TextureCoordOffset(stage),
        m_vertexDesc.TextureCoordSize(stage), 3, reinterpret_cast<float*>(&uvws[0]), count, false);
    return uvws;
}

error GeometryData::SetPaletteIndexArray(const std::vector<unsigned>& palette_array)
{
    return SetVertexMemoryDataArray(0, m_vertexDesc.PaletteIndexOffset(), 1, 1,
        reinterpret_cast<const float*>(&palette_array[0]), static_cast<unsigned>(palette_array.size()), false);
}

std::vector<unsigned> GeometryData::GetPaletteIndexArray(unsigned count)
{
    return GetPaletteIndexArray(0, count);
}

std::vector<unsigned> GeometryData::GetPaletteIndexArray(unsigned offset, unsigned count)
{
    std::vector<unsigned> indices;
    indices.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.PaletteIndexOffset(),
        1, 1, reinterpret_cast<float*>(&indices[0]), count, false);
    return indices;
}

error GeometryData::SetSkinWeightArray(unsigned weight_idx, const std::vector<float>& weight_array)
{
    return SetVertexMemoryDataArray(0, m_vertexDesc.WeightOffset(weight_idx), 1, 1,
        &weight_array[0], static_cast<unsigned>(weight_array.size()), false);
}

error GeometryData::SetTotalSkinWeightArray(const std::vector<float>& weight_array)
{
    if (m_vertexDesc.BlendWeightCount() > 0)
    {
        return SetVertexMemoryDataArray(0, m_vertexDesc.WeightOffset(), m_vertexDesc.BlendWeightCount(),
            m_vertexDesc.BlendWeightCount(), &weight_array[0],
            static_cast<unsigned>(weight_array.size()) / m_vertexDesc.BlendWeightCount(), false);
    }
    return ErrorCode::ok;
}

std::vector<float> GeometryData::GetTotalSkinWeightArray(unsigned vtx_count)
{
    return GetTotalSkinWeightArray(0, vtx_count);
}

std::vector<float> GeometryData::GetTotalSkinWeightArray(unsigned offset, unsigned vtx_count)
{
    std::vector<float> weights;
    weights.resize(static_cast<size_t>(vtx_count) * m_vertexDesc.BlendWeightCount());
    GetVertexMemoryDataArray(offset, m_vertexDesc.WeightOffset(),
        m_vertexDesc.BlendWeightCount(), m_vertexDesc.BlendWeightCount(), &weights[0], vtx_count, false);
    return weights;
}

error GeometryData::SetVertexTangentArray(const std::vector<MathLib::Vector4>& tangents)
{
    return SetVertexMemoryDataArray(0, m_vertexDesc.TangentOffset(), m_vertexDesc.TangentDimension(), 4,
        reinterpret_cast<const float*>(&tangents[0]), static_cast<unsigned>(tangents.size()), false);
}

std::vector<Vector4> GeometryData::GetVertexTangentArray(unsigned count)
{
    return GetVertexTangentArray(0, count);
}

std::vector<Vector4> GeometryData::GetVertexTangentArray(unsigned offset, unsigned count)
{
    std::vector<Vector4> tangents;
    tangents.resize(count);
    GetVertexMemoryDataArray(offset, m_vertexDesc.TangentOffset(),
        m_vertexDesc.TangentDimension(), 4, reinterpret_cast<float*>(&tangents[0]), count, true);
    return tangents;
}

error GeometryData::SetIndexArray(const std::vector<unsigned>& idx_ary)
{
    assert(idx_ary.size() > 0);

    // 要設index array, 必須有index memory
    if (FATAL_LOG_EXPR(m_indexMemory.size() == 0)) return ErrorCode::nullMemoryBuffer;
    unsigned int data_count = m_idxUsedCount;
    if (idx_ary.size() < data_count) data_count = static_cast<unsigned>(idx_ary.size());
    memcpy(&m_indexMemory[0], &idx_ary[0], data_count * sizeof(unsigned int));
    return ErrorCode::ok;
}

IVertexBuffer::ranged_buffer GeometryData::GetRangedVertexMemory(unsigned offset, unsigned count)
{
    unsigned int byte_offset = offset * m_vertexDesc.TotalVertexSize();
    unsigned int byte_count = count * m_vertexDesc.TotalVertexSize();
    byte_buffer buffer;
    buffer.resize(byte_count);
    memcpy(&buffer[0], &m_vertexMemory[byte_offset], byte_count);
    return { offset, count, buffer };
}

IIndexBuffer::ranged_buffer GeometryData::GetRangedIndexMemory(unsigned offset, unsigned count)
{
    uint_buffer buffer;
    buffer.resize(count);
    memcpy(&buffer[0], &m_indexMemory[offset], count * sizeof(unsigned));
    return { offset, count, buffer };
}

error GeometryData::GetVertexMemoryData(unsigned vtxIndex, int elementOffset, int elementDimension,
    int destDimension, float* dest, bool isPos)
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

error GeometryData::SetVertexMemoryData(unsigned vtxIndex, int elementOffset, int elementDimension,
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

error GeometryData::GetVertexMemoryDataArray(unsigned start, int elementOffset, int elementDimension,
    int destDimension, float* dest, unsigned count, bool isPos)
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
    float* src = reinterpret_cast<float*>(&m_vertexMemory[(elementOffset + step * start) * sizeof(float)]);
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

error GeometryData::SetVertexMemoryDataArray(unsigned start, int elementOffset, int elementDimension,
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

const GeometrySegment& GeometryData::GetSegment(unsigned index) const
{
    assert(index < m_geoSegmentVector.size());
    return m_geoSegmentVector[index];
}

void GeometryData::ChangeSegment(unsigned index, unsigned start_vtx, unsigned vtx_count, unsigned start_idx, unsigned idx_count)
{
    if (FATAL_LOG_EXPR(index >= m_geoSegmentVector.size())) return;
    m_geoSegmentVector[index].m_startVtx = start_vtx;
    m_geoSegmentVector[index].m_vtxCount = vtx_count;
    m_geoSegmentVector[index].m_startIdx = start_idx;
    m_geoSegmentVector[index].m_idxCount = idx_count;
}

void GeometryData::CalculateBoundingVolume(bool axis_align)
{
    if (FATAL_LOG_EXPR(m_vertexMemory.size() == 0)) return;

    if (axis_align)
    {
        m_geometryBound = BoundingVolume(ContainmentBox3::ComputeAlignedBox(
            reinterpret_cast<float*>(&m_vertexMemory[0]), SizeofVertex() / sizeof(float),
            GetUsedVertexCount()));
    }
    else
    {
        m_geometryBound = BoundingVolume(ContainmentBox3::ComputeOrientedBox(
            reinterpret_cast<float*>(&m_vertexMemory[0]), SizeofVertex() / sizeof(float),
            GetUsedVertexCount()));
    }
}

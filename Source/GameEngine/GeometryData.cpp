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

GeometryData::GeometryData(const std::string& name)
{
    m_name = name;
    m_vtxCapacity = m_idxCapacity = 0;
    m_vtxUsedCount = m_idxUsedCount = 0;
    m_geoSegmentVector.reserve(8);
    m_topology = PrimitiveTopology::Topology_Undefine;

    m_geometryBound = BoundingVolume{Box3::UNIT_BOX};
}

GeometryData::GeometryData(const GeometryDataContract& contract)
{
    m_name = contract.Name();
    CreateVertexCapacity(contract.VertexFormat(), contract.VertexCapacity(), contract.VertexUsedCount(),
                         contract.IndexCapacity(), contract.IndexUsedCount());
    for (unsigned i = 0; i < contract.Segments().size(); i += 4)
    {
        m_geoSegmentVector.emplace_back(contract.Segments()[i], contract.Segments()[i + 1],
            contract.Segments()[i + 2], contract.Segments()[i + 3]);
    }
    m_topology = static_cast<PrimitiveTopology>(contract.Topology());
    if (contract.Position3s())
    {
        SetPosition3Array(contract.Position3s().value());
    }
    if (contract.Position4s())
    {
        SetPosition4Array(contract.Position4s().value());
    }
    if (contract.Normals())
    {
        SetVertexNormalArray(contract.Normals().value());
    }
    if (contract.DiffuseColors())
    {
        SetDiffuseColorArray(contract.DiffuseColors().value());
    }
    if (contract.SpecularColors())
    {
        SetSpecularColorArray(contract.SpecularColors().value());
    }
    for (unsigned i = 0; i < contract.TextureCoords().size(); i++)
    {
        TextureCoordContract coord = TextureCoordContract::FromContract(contract.TextureCoords()[i]);
        if (coord.Texture2DCoords())
        {
            SetTexture2DCoordArray(i, coord.Texture2DCoords().value());
        }
        else if (coord.Texture1DCoords())
        {
            SetTexture1DCoordArray(i, coord.Texture1DCoords().value());
        }
        else if (coord.Texture3DCoords())
        {
            SetTexture3DCoordArray(i, coord.Texture3DCoords().value());
        }
    }
    if (contract.PaletteIndices())
    {
        SetPaletteIndexArray(contract.PaletteIndices().value());
    }
    if (contract.Weights())
    {
        SetTotalSkinWeightArray(contract.Weights().value());
    }
    if (contract.Tangents())
    {
        SetVertexTangentArray(contract.Tangents().value());
    }
    if (contract.Indices())
    {
        SetIndexArray(contract.Indices().value());
    }
    m_geometryBound = BoundingVolume(BoundingVolumeContract::FromContract(contract.GeometryBound()));
}

GeometryData::~GeometryData()
{

}

GeometryDataContract GeometryData::SerializeContract()
{
    GeometryDataContract contract;
    contract.Name() = m_name;
    contract.VertexFormat() = m_vertexFormatCode.ToString();
    for (unsigned i = 0; i < m_geoSegmentVector.size(); i++)
    {
        contract.Segments().emplace_back(m_geoSegmentVector[i].m_startVtx);
        contract.Segments().emplace_back(m_geoSegmentVector[i].m_vtxCount);
        contract.Segments().emplace_back(m_geoSegmentVector[i].m_startIdx);
        contract.Segments().emplace_back(m_geoSegmentVector[i].m_idxCount);
    }
    contract.Topology() = static_cast<unsigned>(m_topology);
    if (m_vertexDesc.HasPosition3())
    {
        contract.Position3s() = GetPosition3Array(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasPosition4())
    {
        contract.Position4s() = GetPosition4Array(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasNormal())
    {
        contract.Normals() = GetVertexNormalArray(m_vtxUsedCount);
    }
    if (m_vertexDesc.HasDiffuseColor(VertexDescription::ColorNumeric::Float))
    {
        contract.DiffuseColors() = GetDiffuseColorArray(VertexDescription::ColorNumeric::Float, m_vtxUsedCount);
    }
    if (m_vertexDesc.HasSpecularColor(VertexDescription::ColorNumeric::Float))
    {
        contract.SpecularColors() = GetSpecularColorArray(VertexDescription::ColorNumeric::Float, m_vtxUsedCount);
    }
}

error GeometryData::CreateVertexCapacity(const std::string& vertex_format_string, unsigned vtx_capa, unsigned vtx_count, unsigned idx_capa, unsigned idx_count)
{
    m_vertexFormatCode.FromString(vertex_format_string);
    m_vertexDesc = m_vertexFormatCode.CalculateVertexSize();

    if (vtx_capa)
    {
        m_vertexMemory.resize(vtx_capa * m_vertexDesc.TotalVertexSize());
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
        m_vertexMemory.resize(vtx_capa * m_vertexDesc.TotalVertexSize());
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

error GeometryData::SetTexture3DCoordArray(unsigned stage, const std::vector<MathLib::Vector3>& uvws)
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return SetVertexMemoryDataArray(0, m_vertexDesc.TextureCoordOffset(stage), m_vertexDesc.TextureCoordSize(stage), 3,
        (const float*)(&uvws[0]), static_cast<unsigned>(uvws.size()), false);
}

error GeometryData::SetPaletteIndexArray(const std::vector<unsigned>& palette_array)
{
    return SetVertexMemoryDataArray(0, m_vertexDesc.PaletteIndexOffset(), 1, 1,
        reinterpret_cast<const float*>(&palette_array[0]), static_cast<unsigned>(palette_array.size()), false);
}

error GeometryData::SetSkinWeightArray(unsigned weight_idx, const std::vector<float>& weight_array)
{
    return SetVertexMemoryDataArray(0, m_vertexDesc.WeightOffset() + weight_idx, 1, 1,
        &weight_array[0], static_cast<unsigned>(weight_array.size()), false,
        [this, weight_idx]()->bool { return m_vertexDesc.BlendWeightCount() <= (int)weight_idx; });
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

error GeometryData::SetVertexTangentArray(const std::vector<MathLib::Vector4>& tangents)
{
    return SetVertexMemoryDataArray(0, m_vertexDesc.TangentOffset(), m_vertexDesc.TangentDimension(), 4,
        reinterpret_cast<const float*>(&tangents[0]), static_cast<unsigned>(tangents.size()), false,
        [this]()->bool { return m_vertexDesc.TangentDimension() != 4; });
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
            float* mem = (float*)(&m_vertexMemory[(base_idx + 3) * sizeof(float)]);
            *mem = 1.0f;
        }
    }

    return ErrorCode::ok;
}

error GeometryData::GetVertexMemoryDataArray(unsigned start, int elementOffset, int elementDimension,
    int destDimension, float* dest, unsigned count, bool isPos, std::function<bool()> skip_check)
{
    assert(dest);
    assert(count > 0);

    if (FATAL_LOG_EXPR(m_vertexMemory.size() == 0)) return ErrorCode::nullMemoryBuffer;
    if (FATAL_LOG_EXPR(m_vertexDesc.TotalVertexSize() == 0)) return ErrorCode::zeroVertexSize;

    if ((skip_check) && (skip_check())) return ErrorCode::ok;
    if (elementOffset < 0)
    {
        memset(dest, 0, count * destDimension * sizeof(float));
        return ErrorCode::ok;
    }

    unsigned int step = m_vertexDesc.TotalVertexSize() / sizeof(float);
    unsigned int pos_count = m_vtxUsedCount - start;
    if (count < pos_count) pos_count = count;
    float* src = (float*)&m_vertexMemory[(elementOffset + step * start) * sizeof(float)];
    int cp_dimension = destDimension;
    if (cp_dimension > elementDimension) cp_dimension = elementDimension;
    bool set_vec4 = false;
    if ((isPos) && (destDimension == 4) && (elementDimension == 3)) set_vec4 = true;
    for (unsigned int i = 0; i < pos_count; i++)
    {
        memcpy(&dest[i * destDimension], src, cp_dimension * sizeof(float));
        if (set_vec4) dest[i * destDimension + 3] = 1.0f;
        src += step;
    }

    return ErrorCode::ok;
}

error GeometryData::SetVertexMemoryDataArray(unsigned start, int elementOffset, int elementDimension,
    int srcDimension, const float* src, unsigned count, bool isPos, std::function<bool()> skip_check)
{
    assert(src);
    assert(count > 0);

    if (FATAL_LOG_EXPR(m_vertexMemory.size() == 0)) return ErrorCode::nullMemoryBuffer;
    if (FATAL_LOG_EXPR(m_vertexDesc.TotalVertexSize() == 0)) return ErrorCode::zeroVertexSize;

    if ((skip_check) && (skip_check())) return ErrorCode::ok;
    if (elementOffset < 0) return ErrorCode::ok;  // no need set

    unsigned int step = m_vertexDesc.TotalVertexSize() / sizeof(float);
    unsigned int pos_count = m_vtxUsedCount - start;
    if (count < pos_count) pos_count = count;
    float* dst = (float*)&m_vertexMemory[(elementOffset + step * start) * sizeof(float)];
    int cp_dimension = srcDimension;
    if (cp_dimension > elementDimension) cp_dimension = elementDimension;
    bool get_vec4 = false;
    if ((isPos) && (srcDimension == 3) && (elementDimension == 4)) get_vec4 = true;
    for (unsigned int i = 0; i < pos_count; i++)
    {
        memcpy(dst, &src[i * srcDimension], cp_dimension * sizeof(float));
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

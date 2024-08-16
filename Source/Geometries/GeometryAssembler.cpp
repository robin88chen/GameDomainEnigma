#include "GeometryAssembler.h"
#include "GeometryData.h"
#include "TextureCoordinateAssembler.h"
#include "MathLib/ContainmentBox3.h"
#include <cassert>

using namespace Enigma::Geometries;
using namespace Enigma::Graphics;

static std::string TOKEN_ID = "Id";
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
static std::array<std::string, Enigma::Graphics::VertexFormatCode::MAX_TEX_COORD> TOKEN_TEX_COORDS =
{
    TOKEN_TEX_COORD0, TOKEN_TEX_COORD1, TOKEN_TEX_COORD2, TOKEN_TEX_COORD3,
    TOKEN_TEX_COORD4, TOKEN_TEX_COORD5, TOKEN_TEX_COORD6, TOKEN_TEX_COORD7
};

GeometryAssembler::GeometryAssembler(const GeometryId& id) : m_id(id), m_factoryDesc(GeometryData::TYPE_RTTI), m_vtxCapacity(0), m_idxCapacity(0), m_vtxUsedCount(0), m_idxUsedCount(0), m_topology(0)
{
}

GeometryAssembler::GeometryAssembler(const GeometryId& id, const Engine::FactoryDesc& desc) : m_id(id), m_factoryDesc(desc), m_vtxCapacity(0), m_idxCapacity(0), m_vtxUsedCount(0), m_idxUsedCount(0), m_topology(0)
{
}

Enigma::Engine::GenericDto GeometryAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    serializeNonVertexAttributesToGenericDto(dto);
    if (m_position3s)
    {
        dto.addOrUpdate(TOKEN_POSITIONS_3, m_position3s.value());
    }
    if (m_position4s)
    {
        dto.addOrUpdate(TOKEN_POSITIONS_4, m_position4s.value());
    }
    if (m_normals)
    {
        dto.addOrUpdate(TOKEN_NORMALS, m_normals.value());
    }
    if (m_diffuseColors)
    {
        dto.addOrUpdate(TOKEN_DIFFUSE_COLORS, m_diffuseColors.value());
    }
    if (m_specularColors)
    {
        dto.addOrUpdate(TOKEN_SPECULAR_COLORS, m_specularColors.value());
    }
    for (unsigned i = 0; i < m_texCoords.size(); i++)
    {
        dto.addOrUpdate(TOKEN_TEX_COORDS[i], m_texCoords[i]);
    }
    if (m_paletteIndices)
    {
        dto.addOrUpdate(TOKEN_PALETTE_INDICES, m_paletteIndices.value());
    }
    if (m_weights)
    {
        dto.addOrUpdate(TOKEN_WEIGHTS, m_weights.value());
    }
    if (m_tangents)
    {
        dto.addOrUpdate(TOKEN_TANGENTS, m_tangents.value());
    }
    if (m_indices)
    {
        dto.addOrUpdate(TOKEN_INDICES, m_indices.value());
    }
    return dto;
}

void GeometryAssembler::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    m_factoryDesc.ClaimAsResourceAsset(name, filename, path_id);
}

void GeometryAssembler::position3s(const std::vector<MathLib::Vector3>& positions)
{
    assert(!m_position4s.has_value());
    m_position3s = positions;
    m_vertexFormat.addPosition3();
}

void GeometryAssembler::position4s(const std::vector<MathLib::Vector4>& positions)
{
    assert(!m_position3s.has_value());
    m_position4s = positions;
    m_vertexFormat.addPosition4();
}

void GeometryAssembler::normals(const std::vector<MathLib::Vector3>& normals)
{
    m_normals = normals;
    m_vertexFormat.addNormal();
}

void GeometryAssembler::diffuseColors(const std::vector<MathLib::Vector4>& colors)
{
    m_diffuseColors = colors;
    m_vertexFormat.addFloatDiffuse();
}

void GeometryAssembler::specularColors(const std::vector<MathLib::Vector4>& colors)
{
    m_specularColors = colors;
    m_vertexFormat.addFloatSpecular();
}

void GeometryAssembler::addTexture2DCoords(const std::vector<MathLib::Vector2>& coords)
{
    size_t tex_stage_count = m_texCoords.size();
    std::shared_ptr<TextureCoordinateAssembler> tex_assembler = std::make_shared<TextureCoordinateAssembler>();
    tex_assembler->texture2DCoords(coords);
    m_texCoords.emplace_back(tex_assembler->assemble());
    m_vertexFormat.addTextureCoord(static_cast<unsigned>(tex_stage_count), 2);
}

void GeometryAssembler::addTexture1DCoords(const std::vector<float>& coords)
{
    size_t tex_stage_count = m_texCoords.size();
    std::shared_ptr<TextureCoordinateAssembler> tex_assembler = std::make_shared<TextureCoordinateAssembler>();
    tex_assembler->texture1DCoords(coords);
    m_texCoords.emplace_back(tex_assembler->assemble());
    m_vertexFormat.addTextureCoord(static_cast<unsigned>(tex_stage_count), 1);
}

void GeometryAssembler::addTexture3DCoords(const std::vector<MathLib::Vector3>& coords)
{
    size_t tex_stage_count = m_texCoords.size();
    std::shared_ptr<TextureCoordinateAssembler> tex_assembler = std::make_shared<TextureCoordinateAssembler>();
    tex_assembler->texture3DCoords(coords);
    m_texCoords.emplace_back(tex_assembler->assemble());
    m_vertexFormat.addTextureCoord(static_cast<unsigned>(tex_stage_count), 3);
}

void GeometryAssembler::paletteIndices(const std::vector<unsigned int>& indices)
{
    m_paletteIndices = indices;
    m_vertexFormat.addPaletteIndex();
}

void GeometryAssembler::weights(const std::vector<float>& weights, unsigned weight_count)
{
    m_weights = weights;
    m_vertexFormat.addWeights(weight_count);
}

void GeometryAssembler::tangents(const std::vector<MathLib::Vector4>& tangents)
{
    m_tangents = tangents;
    m_vertexFormat.addTangent();
}

void GeometryAssembler::computeAlignedBox()
{
    if (auto& pos3 = m_position3s)
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos3.value()[0]), static_cast<unsigned>(pos3.value().size()));
        m_geometryBound = Engine::BoundingVolume{ box };
    }
    else if (auto& pos4 = m_position4s)
    {
        const MathLib::Box3 box = MathLib::ContainmentBox3::ComputeAlignedBox(&(pos4.value()[0]), static_cast<unsigned>(pos4.value().size()));
        m_geometryBound = Engine::BoundingVolume{ box };
    }
}

void GeometryAssembler::serializeNonVertexAttributesToGenericDto(Engine::GenericDto& dto) const
{
    dto.addOrUpdate(TOKEN_ID, m_id.name());
    dto.addOrUpdate(TOKEN_VERTEX_FORMAT, m_vertexFormat.toString());
    dto.addOrUpdate(TOKEN_SEGMENTS, m_segments);
    dto.addOrUpdate(TOKEN_VERTEX_CAPACITY, m_vtxCapacity);
    dto.addOrUpdate(TOKEN_INDEX_CAPACITY, m_idxCapacity);
    dto.addOrUpdate(TOKEN_VERTEX_USED_COUNT, m_vtxUsedCount);
    dto.addOrUpdate(TOKEN_INDEX_USED_COUNT, m_idxUsedCount);
    dto.addOrUpdate(TOKEN_TOPOLOGY, m_topology);
    dto.addOrUpdate(TOKEN_GEOMETRY_BOUND, m_geometryBound.serializeDto().toGenericDto());
}

void GeometryAssembler::addSegment(const GeometrySegment& segment)
{
    m_segments.push_back(segment.m_startVtx);
    m_segments.push_back(segment.m_vtxCount);
    m_segments.push_back(segment.m_startIdx);
    m_segments.push_back(segment.m_idxCount);
}

void GeometryAssembler::segments(const std::vector<GeometrySegment>& segments)
{
    m_segments.clear();
    for (const auto& segment : segments)
    {
        addSegment(segment);
    }
}

GeometryDisassembler::GeometryDisassembler() : m_factoryDesc(GeometryData::TYPE_RTTI), m_vtxCapacity(0), m_idxCapacity(0), m_vtxUsedCount(0), m_idxUsedCount(0), m_topology(PrimitiveTopology::Topology_Undefine)
{
}

void GeometryDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    deserializeNonVertexAttributesFromGenericDto(dto);
    if (dto.hasValue(TOKEN_POSITIONS_3))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector3>>(TOKEN_POSITIONS_3)) m_position3s = v.value();
    }
    if (dto.hasValue(TOKEN_POSITIONS_4))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector4>>(TOKEN_POSITIONS_4)) m_position4s = v.value();
    }
    if (dto.hasValue(TOKEN_NORMALS))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector3>>(TOKEN_NORMALS)) m_normals = v.value();
    }
    if (dto.hasValue(TOKEN_DIFFUSE_COLORS))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector4>>(TOKEN_DIFFUSE_COLORS)) m_diffuseColors = v.value();
    }
    if (dto.hasValue(TOKEN_SPECULAR_COLORS))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector4>>(TOKEN_SPECULAR_COLORS)) m_specularColors = v.value();
    }
    for (auto& token_tex_coord : TOKEN_TEX_COORDS)
    {
        if (dto.hasValue(token_tex_coord))
        {
            if (auto v = dto.tryGetValue<Engine::GenericDto>(token_tex_coord))
            {
                TextureCoordinateDisassembler tex_coord_disassembler;
                tex_coord_disassembler.disassemble(v.value());
                if (m_textureCoordinates)
                {
                    m_textureCoordinates->emplace_back(tex_coord_disassembler);
                }
                else
                {
                    m_textureCoordinates = std::vector<TextureCoordinateDisassembler>{ tex_coord_disassembler };
                }
            }
        }
    }
    if (dto.hasValue(TOKEN_PALETTE_INDICES))
    {
        if (auto v = dto.tryGetValue<std::vector<unsigned>>(TOKEN_PALETTE_INDICES)) m_paletteIndices = v.value();
    }
    if (dto.hasValue(TOKEN_WEIGHTS))
    {
        if (auto v = dto.tryGetValue<std::vector<float>>(TOKEN_WEIGHTS)) m_weights = v.value();
    }
    if (dto.hasValue(TOKEN_TANGENTS))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector4>>(TOKEN_TANGENTS)) m_tangents = v.value();
    }
    if (dto.hasValue(TOKEN_INDICES))
    {
        if (auto v = dto.tryGetValue<std::vector<unsigned>>(TOKEN_INDICES)) m_indices = v.value();
    }
}

void GeometryDisassembler::deserializeNonVertexAttributesFromGenericDto(const Engine::GenericDto& dto)
{
    if (auto v = dto.tryGetValue<std::string>(TOKEN_ID)) m_id = GeometryId{ v.value() };
    if (auto v = dto.tryGetValue<std::string>(TOKEN_VERTEX_FORMAT)) m_vertexFormat.fromString(v.value());
    if (auto v = dto.tryGetValue<std::vector<unsigned>>(TOKEN_SEGMENTS))
    {
        assert(v.value().size() % 4 == 0);
        if (v.value().size() > 0)
        {
            for (size_t i = 0; i < v.value().size() - 3; i += 4)
            {
                m_segments.emplace_back(v.value()[i], v.value()[i + 1], v.value()[i + 2], v.value()[i + 3]);
            }
        }
    }
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_VERTEX_CAPACITY)) m_vtxCapacity = v.value();
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_INDEX_CAPACITY)) m_idxCapacity = v.value();
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_VERTEX_USED_COUNT)) m_vtxUsedCount = v.value();
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_INDEX_USED_COUNT)) m_idxUsedCount = v.value();
    if (auto v = dto.tryGetValue<unsigned>(TOKEN_TOPOLOGY)) m_topology = static_cast<Graphics::PrimitiveTopology>(v.value());
    if (auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_GEOMETRY_BOUND)) m_geometryBound = Engine::BoundingVolume(Engine::BoundingVolumeDto::fromGenericDto(v.value()));
}

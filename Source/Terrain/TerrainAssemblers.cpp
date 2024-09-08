#include "TerrainAssemblers.h"
#include "TerrainPawn.h"
#include "SceneGraph/Spatial.h"

using namespace Enigma::Terrain;
using namespace Enigma::SceneGraph;

/*TerrainGeometryAssembler::TerrainGeometryAssembler(const Geometries::GeometryId& id)
{
    m_id = id;
    //m_dto.id() = id;
    //m_dto.numRows() = 8;
    //m_dto.numCols() = 8;
    //m_dto.minPosition() = MathLib::Vector3(-1.0f, 0.0f, -1.0f);
    //m_dto.maxPosition() = MathLib::Vector3(1.0f, 0.0f, 1.0f);
    //m_dto.minTextureCoordinate() = MathLib::Vector2(0.0f, 0.0f);
    //m_dto.maxTextureCoordinate() = MathLib::Vector2(1.0f, 1.0f);
    //m_dto.heightMap() = std::nullopt;
    m_geometryBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
}

TerrainGeometryAssembler& TerrainGeometryAssembler::numRows(unsigned num_rows)
{
    //m_dto.numRows() = num_rows;
    return *this;
}

TerrainGeometryAssembler& TerrainGeometryAssembler::numCols(unsigned num_cols)
{
    //m_dto.numCols() = num_cols;
    return *this;
}

TerrainGeometryAssembler& TerrainGeometryAssembler::minPosition(const MathLib::Vector3& min_position)
{
    //m_dto.minPosition() = min_position;
    //m_geometryBound = m_dto.calculateGeometryBounding();
    return *this;
}

TerrainGeometryAssembler& TerrainGeometryAssembler::maxPosition(const MathLib::Vector3& max_position)
{
    //m_dto.maxPosition() = max_position;
    //m_geometryBound = m_dto.calculateGeometryBounding();
    return *this;
}

TerrainGeometryAssembler& TerrainGeometryAssembler::heightMap(const float_buffer& height_map)
{
    //m_dto.heightMap() = height_map;
    //m_geometryBound = m_dto.calculateGeometryBounding();
    return *this;
}

TerrainGeometryAssembler& TerrainGeometryAssembler::minTextureCoordinate(const MathLib::Vector2& min_texture_coordinate)
{
    //m_dto.minTextureCoordinate() = min_texture_coordinate;
    return *this;
}

TerrainGeometryAssembler& TerrainGeometryAssembler::maxTextureCoordinate(const MathLib::Vector2& max_texture_coordinate)
{
    //m_dto.maxTextureCoordinate() = max_texture_coordinate;
    return *this;
}

TerrainGeometryAssembler& TerrainGeometryAssembler::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    //m_dto.factoryDesc().ClaimAsResourceAsset(name, filename, path_id);
    return *this;
}

Enigma::Engine::GenericDto TerrainGeometryAssembler::toGenericDto() const
{
    return Engine::GenericDto(); // dto().toGenericDto();
}

TerrainGeometryDto TerrainGeometryAssembler::dto() const
{
    TerrainGeometryDto& geo = const_cast<TerrainGeometryDto&>(m_dto);
    geo.geometryBound() = m_geometryBound.serializeDto().toGenericDto();
    return m_dto;
}*/

//------------------------------------------------------------------------------
/*TerrainPrimitiveAssembler::TerrainPrimitiveAssembler(const Primitives::PrimitiveId& id) : m_meshPrimitiveAssembler(id)
{
    m_id = id;
}

Enigma::Renderables::MeshPrimitiveAssembler& TerrainPrimitiveAssembler::meshPrimitive()
{
    return m_meshPrimitiveAssembler;
}

TerrainPrimitiveAssembler& TerrainPrimitiveAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

TerrainPrimitiveDto TerrainPrimitiveAssembler::dto() const
{
    TerrainPrimitiveDto terrain_dto = TerrainPrimitiveDto(m_meshPrimitiveAssembler.dto());
    terrain_dto.factoryDesc() = m_dto.factoryDesc();
    return terrain_dto;
}

Enigma::Engine::GenericDto TerrainPrimitiveAssembler::toGenericDto() const
{
    return dto().toGenericDto();
}*/

//------------------------------------------------------------------------------
/*TerrainPawnAssembler::TerrainPawnAssembler(const SpatialId& id) : m_pawnAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(TerrainPawn::TYPE_RTTI.getName()));
    m_pawnAssembler.factory(m_dto.factoryDesc());
}
PawnAssembler& TerrainPawnAssembler::pawn()
{
    return m_pawnAssembler;
}

TerrainPawnAssembler& TerrainPawnAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_pawnAssembler.factory(factory);
    return *this;
}

TerrainPawnAssembler& TerrainPawnAssembler::asNative(const std::string& file_at_path)
{
    Engine::FactoryDesc fd = m_dto.factoryDesc();
    fd.ClaimAsNative(file_at_path);
    m_dto.factoryDesc(fd);
    m_pawnAssembler.factory(m_dto.factoryDesc());
    return *this;
}

TerrainPawnDto TerrainPawnAssembler::dto() const
{
    TerrainPawnDto pawn_dto(m_pawnAssembler.toGenericDto());
    pawn_dto.id(m_dto.id());
    pawn_dto.factoryDesc(m_dto.factoryDesc());
    return pawn_dto;
}

Enigma::Engine::GenericDto TerrainPawnAssembler::toGenericDto() const
{
    return dto().toGenericDto();
}*/

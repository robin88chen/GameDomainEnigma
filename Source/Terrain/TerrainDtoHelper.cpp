#include "TerrainDtoHelper.h"
#include "TerrainPawn.h"
#include "SceneGraph/Spatial.h"

using namespace Enigma::Terrain;
using namespace Enigma::SceneGraph;

TerrainGeometryDtoHelper::TerrainGeometryDtoHelper(const std::string& name)
{
    m_dto.id() = Engine::GeometryId{ name };
    m_dto.numRows() = 8;
    m_dto.numCols() = 8;
    m_dto.minPosition() = MathLib::Vector3(-1.0f, 0.0f, -1.0f);
    m_dto.maxPosition() = MathLib::Vector3(1.0f, 0.0f, 1.0f);
    m_dto.minTextureCoordinate() = MathLib::Vector2(0.0f, 0.0f);
    m_dto.maxTextureCoordinate() = MathLib::Vector2(1.0f, 1.0f);
    m_dto.heightMap() = std::nullopt;
    m_geometryBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::numRows(unsigned num_rows)
{
    m_dto.numRows() = num_rows;
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::numCols(unsigned num_cols)
{
    m_dto.numCols() = num_cols;
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::minPosition(const MathLib::Vector3& min_position)
{
    m_dto.minPosition() = min_position;
    m_geometryBound = m_dto.calculateGeometryBounding();
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::maxPosition(const MathLib::Vector3& max_position)
{
    m_dto.maxPosition() = max_position;
    m_geometryBound = m_dto.calculateGeometryBounding();
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::heightMap(const float_buffer& height_map)
{
    m_dto.heightMap() = height_map;
    m_geometryBound = m_dto.calculateGeometryBounding();
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::minTextureCoordinate(const MathLib::Vector2& min_texture_coordinate)
{
    m_dto.minTextureCoordinate() = min_texture_coordinate;
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::maxTextureCoordinate(const MathLib::Vector2& max_texture_coordinate)
{
    m_dto.maxTextureCoordinate() = max_texture_coordinate;
    return *this;
}

Enigma::Engine::GenericDto TerrainGeometryDtoHelper::toGenericDto() const
{
    return toDto().toGenericDto();
}

TerrainGeometryDto TerrainGeometryDtoHelper::toDto() const
{
    TerrainGeometryDto& geo = const_cast<TerrainGeometryDto&>(m_dto);
    geo.geometryBound() = m_geometryBound.serializeDto().toGenericDto();
    return m_dto;
}

//------------------------------------------------------------------------------
TerrainPawnDtoHelper::TerrainPawnDtoHelper(const std::string& name)
{
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(TerrainPawn::TYPE_RTTI.getName());
    m_dto.name() = name;
    m_dto.isTopLevel() = false;
    m_dto.localTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.worldTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    m_dto.graphDepth() = 0;
    m_dto.cullingMode() = static_cast<unsigned>(Spatial::CullingMode::Dynamic);
    m_dto.notifyFlag() = static_cast<unsigned>(Spatial::NotifyBit::Notify_All);
    m_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::localTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.localTransform() = local_transform;
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::worldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.worldTransform() = world_transform;
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::modelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::graphDepth(unsigned graph_depth)
{
    m_dto.graphDepth() = graph_depth;
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::cullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.cullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::notifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.notifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::topLevel(bool top_level)
{
    m_dto.isTopLevel() = top_level;
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::spatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.spatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::terrainPrimitive(const TerrainPrimitiveDto& terrain_primitive_dto)
{
    m_dto.primitive() = terrain_primitive_dto.toGenericDto();
    return *this;
}

Enigma::Engine::GenericDto TerrainPawnDtoHelper::toGenericDto() const
{
    return m_dto.toGenericDto();
}

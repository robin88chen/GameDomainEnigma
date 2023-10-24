#include "TerrainDtoHelper.h"
#include "TerrainPawn.h"
#include "SceneGraph/Spatial.h"

using namespace Enigma::Terrain;
using namespace Enigma::SceneGraph;

TerrainGeometryDtoHelper::TerrainGeometryDtoHelper(const std::string& name)
{
    m_dto.Name() = name;
    m_dto.NumRows() = 8;
    m_dto.NumCols() = 8;
    m_dto.MinPosition() = MathLib::Vector3(-1.0f, 0.0f, -1.0f);
    m_dto.MaxPosition() = MathLib::Vector3(1.0f, 0.0f, 1.0f);
    m_dto.MinTextureCoordinate() = MathLib::Vector2(0.0f, 0.0f);
    m_dto.MaxTextureCoordinate() = MathLib::Vector2(1.0f, 1.0f);
    m_dto.HeightMap() = std::nullopt;
    m_geometryBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::NumRows(unsigned num_rows)
{
    m_dto.NumRows() = num_rows;
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::NumCols(unsigned num_cols)
{
    m_dto.NumCols() = num_cols;
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::MinPosition(const MathLib::Vector3& min_position)
{
    m_dto.MinPosition() = min_position;
    m_geometryBound = m_dto.CalculateGeometryBounding();
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::MaxPosition(const MathLib::Vector3& max_position)
{
    m_dto.MaxPosition() = max_position;
    m_geometryBound = m_dto.CalculateGeometryBounding();
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::HeightMap(const float_buffer& height_map)
{
    m_dto.HeightMap() = height_map;
    m_geometryBound = m_dto.CalculateGeometryBounding();
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::MinTextureCoordinate(const MathLib::Vector2& min_texture_coordinate)
{
    m_dto.MinTextureCoordinate() = min_texture_coordinate;
    return *this;
}

TerrainGeometryDtoHelper& TerrainGeometryDtoHelper::MaxTextureCoordinate(const MathLib::Vector2& max_texture_coordinate)
{
    m_dto.MaxTextureCoordinate() = max_texture_coordinate;
    return *this;
}

Enigma::Engine::GenericDto TerrainGeometryDtoHelper::ToGenericDto() const
{
    return ToDto().ToGenericDto();
}

TerrainGeometryDto TerrainGeometryDtoHelper::ToDto() const
{
    TerrainGeometryDto& geo = const_cast<TerrainGeometryDto&>(m_dto);
    geo.GeometryBound() = m_geometryBound.SerializeDto().ToGenericDto();
    return m_dto;
}

//------------------------------------------------------------------------------
TerrainPawnDtoHelper::TerrainPawnDtoHelper(const std::string& name)
{
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.TheFactoryDesc() = Engine::FactoryDesc(TerrainPawn::TYPE_RTTI.getName());
    m_dto.Name() = name;
    m_dto.IsTopLevel() = false;
    m_dto.LocalTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.WorldTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.ModelBound() = m_modelBound.SerializeDto().ToGenericDto();
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).SerializeDto().ToGenericDto();
    m_dto.GraphDepth() = 0;
    m_dto.CullingMode() = static_cast<unsigned>(Spatial::CullingMode::Dynamic);
    m_dto.NotifyFlag() = static_cast<unsigned>(Spatial::NotifyBit::Notify_All);
    m_dto.SpatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::Factory(const Engine::FactoryDesc& factory)
{
    m_dto.TheFactoryDesc() = factory;
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::LocalTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.LocalTransform() = local_transform;
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::WorldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.WorldTransform() = world_transform;
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).SerializeDto().ToGenericDto();
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::ModelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.ModelBound() = m_modelBound.SerializeDto().ToGenericDto();
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).SerializeDto().ToGenericDto();
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::GraphDepth(unsigned graph_depth)
{
    m_dto.GraphDepth() = graph_depth;
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::CullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.CullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::NotifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.NotifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::TopLevel(bool top_level)
{
    m_dto.IsTopLevel() = top_level;
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::SpatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.SpatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
    return *this;
}

TerrainPawnDtoHelper& TerrainPawnDtoHelper::TerrainPrimitive(const TerrainPrimitiveDto& terrain_primitive_dto)
{
    m_dto.ThePrimitive() = terrain_primitive_dto.ToGenericDto();
    return *this;
}

Enigma::Engine::GenericDto TerrainPawnDtoHelper::ToGenericDto() const
{
    return m_dto.ToGenericDto();
}

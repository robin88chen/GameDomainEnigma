#include "TerrainDtoHelper.h"
#include "TerrainPawn.h"
#include "SceneGraph/Spatial.h"

using namespace Enigma::Terrain;
using namespace Enigma::SceneGraph;

TerrainPawnDtoHelper::TerrainPawnDtoHelper(const std::string& name)
{
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.TheFactoryDesc() = Engine::FactoryDesc(TerrainPawn::TYPE_RTTI.GetName());
    m_dto.Name() = name;
    m_dto.IsTopLevel() = false;
    m_dto.LocalTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.WorldTransform() = MathLib::Matrix4::IDENTITY;
    m_dto.ModelBound() = m_modelBound.SerializeDto().ToGenericDto();
    m_dto.WorldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.WorldTransform()).SerializeDto().ToGenericDto();
    m_dto.GraphDepth() = 0;
    m_dto.CullingMode() = static_cast<unsigned>(Spatial::CullingMode::Dynamic);
    m_dto.NotifyFlag() = static_cast<unsigned>(Spatial::NotifyBit::Notify_None);
    m_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
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

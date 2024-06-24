#include "SpatialAssembler.h"

using namespace Enigma::SceneGraph;

SpatialAssembler::SpatialAssembler(const SpatialId& id)
{
    m_id = id;
    m_dto.id() = id;
    m_modelBound = Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX };
    m_dto.factoryDesc() = Engine::FactoryDesc(Spatial::TYPE_RTTI.getName());
    //m_dto.name() = id.name();
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

SpatialAssembler& SpatialAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc() = factory;
    return *this;
}

SpatialAssembler& SpatialAssembler::localTransform(const MathLib::Matrix4& local_transform)
{
    m_dto.localTransform() = local_transform;
    return *this;
}

SpatialAssembler& SpatialAssembler::worldTransform(const MathLib::Matrix4& world_transform)
{
    m_dto.worldTransform() = world_transform;
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

SpatialAssembler& SpatialAssembler::modelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    m_dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    m_dto.worldBound() = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_dto.worldTransform()).serializeDto().toGenericDto();
    return *this;
}

SpatialAssembler& SpatialAssembler::graphDepth(unsigned graph_depth)
{
    m_dto.graphDepth() = graph_depth;
    return *this;
}

SpatialAssembler& SpatialAssembler::cullingMode(Spatial::CullingMode culling_mode)
{
    m_dto.cullingMode() = static_cast<unsigned>(culling_mode);
    return *this;
}

SpatialAssembler& SpatialAssembler::notifyFlags(Spatial::NotifyFlags notify_flag)
{
    m_dto.notifyFlag() = static_cast<unsigned>(notify_flag.to_ullong());
    return *this;
}

SpatialAssembler& SpatialAssembler::topLevel(bool top_level)
{
    m_dto.isTopLevel() = top_level;
    return *this;
}

SpatialAssembler& SpatialAssembler::spatialFlags(Spatial::SpatialFlags spatial_flag)
{
    m_dto.spatialFlag() = static_cast<unsigned>(spatial_flag.to_ullong());
    return *this;
}

Enigma::Engine::GenericDto SpatialAssembler::toGenericDto() const
{
    return m_dto.toGenericDto();
}

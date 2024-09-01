#include "SpatialAssembler.h"

#include "Culler.h"
#include "GameEngine/BoundingVolumeAssembler.h"

using namespace Enigma::SceneGraph;

static std::string TOKEN_ID = "Id";
static std::string TOKEN_PARENT_NAME = "ParentName";
static std::string TOKEN_LOCAL_TRANSFORM = "LocalTransform";
static std::string TOKEN_WORLD_TRANSFORM = "WorldTransform";
static std::string TOKEN_GRAPH_DEPTH = "GraphDepth";
static std::string TOKEN_WORLD_BOUND = "WorldBound";
static std::string TOKEN_MODEL_BOUND = "ModelBound";
static std::string TOKEN_CULLING_MODE = "CullingMode";
static std::string TOKEN_SPATIAL_FLAG = "SpatialFlag";
static std::string TOKEN_NOTIFY_FLAG = "NotifyFlag";
static std::string TOKEN_PARENT_ID = "ParentId";

SpatialAssembler::SpatialAssembler(const SpatialId& id) : m_id(id), m_factoryDesc(Spatial::TYPE_RTTI), m_isTopLevel(false), m_localTransform(MathLib::Matrix4::IDENTITY), m_worldTransform(MathLib::Matrix4::IDENTITY), m_graphDepth(0), m_modelBound(Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX }), m_worldBound(Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX }), m_cullingMode(Spatial::CullingMode::Dynamic), m_spatialFlag(Spatial::Spatial_Unlit), m_notifyFlag(Spatial::NotifyBit::Notify_All)
{
}

void SpatialAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_factoryDesc = factory;
    //! 不可因為設 factory 而失去原本的 native file path
    if (m_nativeFileAtPath) m_factoryDesc.ClaimAsNative(m_nativeFileAtPath.value());
}

void SpatialAssembler::localTransform(const MathLib::Matrix4& local_transform)
{
    m_localTransform = local_transform;
}

void SpatialAssembler::worldTransform(const MathLib::Matrix4& world_transform)
{
    m_worldTransform = world_transform;
    calculateWorldBound();
}

void SpatialAssembler::modelBound(const Engine::BoundingVolume& model_bound)
{
    m_modelBound = model_bound;
    calculateWorldBound();
}

void SpatialAssembler::calculateWorldBound()
{
    m_worldBound = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_worldTransform);
}

void SpatialAssembler::asNative(const std::string& file_at_path)
{
    m_nativeFileAtPath = file_at_path;
    m_factoryDesc.ClaimAsNative(file_at_path);
}

Enigma::Engine::GenericDto SpatialAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.asTopLevel(m_isTopLevel);
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    dto.addOrUpdate(TOKEN_LOCAL_TRANSFORM, m_localTransform);
    dto.addOrUpdate(TOKEN_WORLD_TRANSFORM, m_worldTransform);
    dto.addOrUpdate(TOKEN_GRAPH_DEPTH, m_graphDepth);
    std::shared_ptr<Engine::BoundingVolumeAssembler> model_bv_assembler = std::make_shared<Engine::BoundingVolumeAssembler>();
    m_modelBound.assemble(model_bv_assembler);
    dto.addOrUpdate(TOKEN_MODEL_BOUND, model_bv_assembler->assemble());
    std::shared_ptr<Engine::BoundingVolumeAssembler> world_bv_assembler = std::make_shared<Engine::BoundingVolumeAssembler>();
    m_worldBound.assemble(world_bv_assembler);
    dto.addOrUpdate(TOKEN_WORLD_BOUND, world_bv_assembler->assemble());
    dto.addOrUpdate(TOKEN_CULLING_MODE, static_cast<unsigned>(m_cullingMode));
    dto.addOrUpdate(TOKEN_SPATIAL_FLAG, static_cast<unsigned>(m_spatialFlag.to_ullong()));
    dto.addOrUpdate(TOKEN_NOTIFY_FLAG, static_cast<unsigned>(m_notifyFlag.to_ullong()));
    if (m_parentId.has_value()) dto.addOrUpdate(TOKEN_PARENT_ID, m_parentId.value().tokens());
    return dto;
}

SpatialDisassembler::SpatialDisassembler() : m_factoryDesc(Spatial::TYPE_RTTI), m_isTopLevel(false), m_localTransform(MathLib::Matrix4::IDENTITY), m_worldTransform(MathLib::Matrix4::IDENTITY), m_graphDepth(0), m_modelBound(Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX }), m_worldBound(Engine::BoundingVolume{ MathLib::Box3::UNIT_BOX }), m_cullingMode(Spatial::CullingMode::Dynamic), m_spatialFlag(Spatial::Spatial_Unlit), m_notifyFlag(Spatial::NotifyBit::Notify_All)
{
}

void SpatialDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    m_isTopLevel = dto.isTopLevel();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) m_id = SpatialId(v.value());
    if (auto v = dto.tryGetValue<MathLib::Matrix4>(TOKEN_LOCAL_TRANSFORM)) m_localTransform = v.value();
    if (auto v = dto.tryGetValue<MathLib::Matrix4>(TOKEN_WORLD_TRANSFORM)) m_worldTransform = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_GRAPH_DEPTH)) m_graphDepth = v.value();
    if (auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_MODEL_BOUND))
    {
        Engine::BoundingVolume bv;
        bv.disassemble(std::make_shared<Engine::BoundingVolumeDisassembler>(v.value()));
        m_modelBound = bv;
    }
    if (auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_WORLD_BOUND))
    {
        Engine::BoundingVolume bv;
        bv.disassemble(std::make_shared<Engine::BoundingVolumeDisassembler>(v.value()));
        m_worldBound = bv;
    }
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_CULLING_MODE)) m_cullingMode = static_cast<Spatial::CullingMode>(v.value());
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_SPATIAL_FLAG)) m_spatialFlag = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_NOTIFY_FLAG)) m_notifyFlag = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PARENT_ID)) m_parentId = SpatialId(v.value());
}

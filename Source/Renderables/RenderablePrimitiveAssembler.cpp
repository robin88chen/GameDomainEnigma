#include "RenderablePrimitiveAssembler.h"
#include "Primitives/PrimitiveQueries.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "ModelPrimitive.h"

using namespace Enigma::Renderables;

MeshPrimitiveAssembler::MeshPrimitiveAssembler(const Primitives::PrimitiveId& id)
{
    m_id = id;
    m_dto.id() = id;
}

MeshPrimitiveAssembler& MeshPrimitiveAssembler::geometryId(const Geometries::GeometryId& id)
{
    m_dto.geometryId() = id;
    return *this;
}

MeshPrimitiveAssembler& MeshPrimitiveAssembler::effect(const Engine::EffectMaterialId& id)
{
    m_dto.effects().emplace_back(id);
    return *this;
}

MeshPrimitiveAssembler& MeshPrimitiveAssembler::textureMap(const Engine::EffectTextureMapAssembler& texture_map)
{
    Engine::EffectTextureMapAssembler texture_map_dto = texture_map;
    m_dto.textureMaps().emplace_back(texture_map_dto.toGenericDto());
    return *this;
}

MeshPrimitiveAssembler& MeshPrimitiveAssembler::renderListID(Renderer::Renderer::RenderListID id)
{
    m_dto.renderListID() = id;
    return *this;
}

MeshPrimitiveAssembler& MeshPrimitiveAssembler::visualTechnique(const std::string& technique)
{
    m_dto.visualTechniqueSelection() = technique;
    return *this;
}

MeshPrimitiveAssembler& MeshPrimitiveAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

Enigma::Engine::GenericDto MeshPrimitiveAssembler::toGenericDto() const
{
    return dto().toGenericDto();
}

std::shared_ptr<MeshPrimitive> MeshPrimitiveAssembler::constitute(Primitives::PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<MeshPrimitive>(std::make_shared<Primitives::RequestPrimitiveConstitution>(m_id, toGenericDto(), persistence_level)->dispatch());
}

MeshPrimitiveDto MeshPrimitiveAssembler::dto() const
{
    return m_dto;
}

SkinMeshPrimitiveAssembler::SkinMeshPrimitiveAssembler(const Primitives::PrimitiveId& id)
{
    m_id = id;
    m_dto.id() = id;
}

SkinMeshPrimitiveAssembler& SkinMeshPrimitiveAssembler::geometryId(const Geometries::GeometryId& id)
{
    m_dto.geometryId() = id;
    return *this;
}

SkinMeshPrimitiveAssembler& SkinMeshPrimitiveAssembler::effect(const Engine::EffectMaterialId& id)
{
    m_dto.effects().emplace_back(id);
    return *this;
}

SkinMeshPrimitiveAssembler& SkinMeshPrimitiveAssembler::textureMap(const Engine::EffectTextureMapAssembler& texture_map)
{
    Engine::EffectTextureMapAssembler texture_map_dto = texture_map;
    m_dto.textureMaps().emplace_back(texture_map_dto.toGenericDto());
    return *this;
}

SkinMeshPrimitiveAssembler& SkinMeshPrimitiveAssembler::renderListID(Renderer::Renderer::RenderListID id)
{
    m_dto.renderListID() = id;
    return *this;
}

SkinMeshPrimitiveAssembler& SkinMeshPrimitiveAssembler::visualTechnique(const std::string& technique)
{
    m_dto.visualTechniqueSelection() = technique;
    return *this;
}

SkinMeshPrimitiveAssembler& SkinMeshPrimitiveAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

Enigma::Engine::GenericDto SkinMeshPrimitiveAssembler::toGenericDto()
{
    return m_dto.toGenericDto();
}

std::shared_ptr<SkinMeshPrimitive> SkinMeshPrimitiveAssembler::constitute(Primitives::PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<SkinMeshPrimitive>(std::make_shared<Primitives::RequestPrimitiveConstitution>(m_id, toGenericDto(), persistence_level)->dispatch());
}

MeshNodeAssembler::MeshNodeAssembler(const std::string& name)
{
    m_name = name;
    m_dto.name() = name;
}

MeshNodeAssembler& MeshNodeAssembler::localT_PosTransform(MathLib::Matrix4& transform)
{
    m_dto.localT_PosTransform() = transform;
    return *this;
}

MeshNodeAssembler& MeshNodeAssembler::parentNode(const std::string& name)
{
    m_parentName = name;
    return *this;
}

MeshNodeAssembler& MeshNodeAssembler::meshPrimitive(const Primitives::PrimitiveId& id)
{
    m_dto.meshPrimitiveId() = id;
    return *this;
}

Enigma::Engine::GenericDto MeshNodeAssembler::toGenericDto()
{
    return m_dto.toGenericDto();
}

MeshNodeTreeAssembler::MeshNodeTreeAssembler()
{
}

MeshNodeTreeAssembler& MeshNodeTreeAssembler::addNode(const MeshNodeAssembler& node)
{
    if (m_nodes.empty())
    {
        m_nodes.emplace_back(node);
    }
    else
    {
        m_nodes.emplace_back(node);
        m_nodes.back().m_dto.parentIndexInArray() = findNodeIndex(m_nodes.back().m_parentName);
    }
    return *this;
}

std::optional<unsigned> MeshNodeTreeAssembler::findNodeIndex(const std::string& name)
{
    if (name.empty()) return std::nullopt;
    for (unsigned i = 0; i < m_nodes.size(); i++)
    {
        if (m_nodes[i].m_name == name)
        {
            return i;
        }
    }
    return std::nullopt;
}

Enigma::Engine::GenericDto MeshNodeTreeAssembler::toGenericDto()
{
    for (auto& node : m_nodes)
    {
        m_dto.meshNodes().emplace_back(node.toGenericDto());
    }
    return m_dto.toGenericDto();
}

ModelPrimitiveAssembler::ModelPrimitiveAssembler(const Primitives::PrimitiveId& id)
{
    m_id = id;
    m_dto.id() = id;
}

ModelPrimitiveAssembler& ModelPrimitiveAssembler::meshNodeTree(const MeshNodeTreeAssembler& tree)
{
    MeshNodeTreeAssembler tree_dto = tree;
    m_dto.nodeTree() = tree_dto.toGenericDto();
    return *this;
}

ModelPrimitiveAssembler& ModelPrimitiveAssembler::animator(const Animators::AnimatorId& id)
{
    m_dto.animatorId() = id;
    return *this;
}

ModelPrimitiveAssembler& ModelPrimitiveAssembler::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

Enigma::Engine::GenericDto ModelPrimitiveAssembler::toGenericDto()
{
    return m_dto.toGenericDto();
}

std::shared_ptr<ModelPrimitive> ModelPrimitiveAssembler::constitute(Primitives::PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<ModelPrimitive>(std::make_shared<Primitives::RequestPrimitiveConstitution>(m_id, toGenericDto(), persistence_level)->dispatch());
}

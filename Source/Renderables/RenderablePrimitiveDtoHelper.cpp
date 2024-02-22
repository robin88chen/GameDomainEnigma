#include "RenderablePrimitiveDtoHelper.h"

using namespace Enigma::Renderables;

MeshPrimitiveDtoHelper::MeshPrimitiveDtoHelper(const Primitives::PrimitiveId& id)
{
    m_dto.id() = id;
}

MeshPrimitiveDtoHelper& MeshPrimitiveDtoHelper::geometryId(const Geometries::GeometryId& id)
{
    m_dto.geometryId() = id;
    return *this;
}

MeshPrimitiveDtoHelper& MeshPrimitiveDtoHelper::effect(const Engine::EffectMaterialId& id)
{
    m_dto.effects().emplace_back(id);
    return *this;
}

MeshPrimitiveDtoHelper& MeshPrimitiveDtoHelper::textureMap(const Engine::EffectTextureMapDtoHelper& texture_map)
{
    Engine::EffectTextureMapDtoHelper texture_map_dto = texture_map;
    m_dto.textureMaps().emplace_back(texture_map_dto.toGenericDto());
    return *this;
}

MeshPrimitiveDtoHelper& MeshPrimitiveDtoHelper::renderListID(Renderer::Renderer::RenderListID id)
{
    m_dto.renderListID() = id;
    return *this;
}

MeshPrimitiveDtoHelper& MeshPrimitiveDtoHelper::visualTechnique(const std::string& technique)
{
    m_dto.visualTechniqueSelection() = technique;
    return *this;
}

MeshPrimitiveDtoHelper& MeshPrimitiveDtoHelper::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

Enigma::Engine::GenericDto MeshPrimitiveDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}

SkinMeshPrimitiveDtoHelper::SkinMeshPrimitiveDtoHelper(const Primitives::PrimitiveId& id)
{
    m_dto.id() = id;
}

SkinMeshPrimitiveDtoHelper& SkinMeshPrimitiveDtoHelper::geometryId(const Geometries::GeometryId& id)
{
    m_dto.geometryId() = id;
    return *this;
}

SkinMeshPrimitiveDtoHelper& SkinMeshPrimitiveDtoHelper::effect(const Engine::EffectMaterialId& id)
{
    m_dto.effects().emplace_back(id);
    return *this;
}

SkinMeshPrimitiveDtoHelper& SkinMeshPrimitiveDtoHelper::textureMap(const Engine::EffectTextureMapDtoHelper& texture_map)
{
    Engine::EffectTextureMapDtoHelper texture_map_dto = texture_map;
    m_dto.textureMaps().emplace_back(texture_map_dto.toGenericDto());
    return *this;
}

SkinMeshPrimitiveDtoHelper& SkinMeshPrimitiveDtoHelper::renderListID(Renderer::Renderer::RenderListID id)
{
    m_dto.renderListID() = id;
    return *this;
}

SkinMeshPrimitiveDtoHelper& SkinMeshPrimitiveDtoHelper::visualTechnique(const std::string& technique)
{
    m_dto.visualTechniqueSelection() = technique;
    return *this;
}

SkinMeshPrimitiveDtoHelper& SkinMeshPrimitiveDtoHelper::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

Enigma::Engine::GenericDto SkinMeshPrimitiveDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}

MeshNodeDtoHelper::MeshNodeDtoHelper(const std::string& name)
{
    m_name = name;
    m_dto.name() = name;
}

MeshNodeDtoHelper& MeshNodeDtoHelper::localT_PosTransform(MathLib::Matrix4& transform)
{
    m_dto.localT_PosTransform() = transform;
    return *this;
}

MeshNodeDtoHelper& MeshNodeDtoHelper::parentNode(const std::string& name)
{
    m_parentName = name;
    return *this;
}

MeshNodeDtoHelper& MeshNodeDtoHelper::meshPrimitive(const Primitives::PrimitiveId& id)
{
    m_dto.meshPrimitiveId() = id;
    return *this;
}

Enigma::Engine::GenericDto MeshNodeDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}

MeshNodeTreeDtoHelper::MeshNodeTreeDtoHelper()
{
}

MeshNodeTreeDtoHelper& MeshNodeTreeDtoHelper::addNode(const MeshNodeDtoHelper& node)
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

std::optional<unsigned> MeshNodeTreeDtoHelper::findNodeIndex(const std::string& name)
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

Enigma::Engine::GenericDto MeshNodeTreeDtoHelper::toGenericDto()
{
    for (auto& node : m_nodes)
    {
        m_dto.meshNodes().emplace_back(node.toGenericDto());
    }
    return m_dto.toGenericDto();
}

ModelPrimitiveDtoHelper::ModelPrimitiveDtoHelper(const Primitives::PrimitiveId& id)
{
    m_dto.id() = id;
}

ModelPrimitiveDtoHelper& ModelPrimitiveDtoHelper::meshNodeTree(const MeshNodeTreeDtoHelper& tree)
{
    MeshNodeTreeDtoHelper tree_dto = tree;
    m_dto.nodeTree() = tree_dto.toGenericDto();
    return *this;
}

ModelPrimitiveDtoHelper& ModelPrimitiveDtoHelper::animator(const Animators::AnimatorId& id)
{
    m_dto.animatorId() = id;
    return *this;
}

ModelPrimitiveDtoHelper& ModelPrimitiveDtoHelper::asNative(const std::string& file_at_path)
{
    m_dto.factoryDesc().ClaimAsNative(file_at_path);
    return *this;
}

Enigma::Engine::GenericDto ModelPrimitiveDtoHelper::toGenericDto()
{
    return m_dto.toGenericDto();
}

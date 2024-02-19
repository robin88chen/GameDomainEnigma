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

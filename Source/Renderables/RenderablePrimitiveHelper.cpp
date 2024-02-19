#include "RenderablePrimitiveHelper.h"
#include "Primitives/PrimitiveQueries.h"

using namespace Enigma::Renderables;

MeshPrimitiveHelper::MeshPrimitiveHelper(const Primitives::PrimitiveId& id) : m_id(id), m_dto(id)
{
}

MeshPrimitiveHelper& MeshPrimitiveHelper::geometryId(const Geometries::GeometryId& id)
{
    m_dto.geometryId(id);
    return *this;
}

MeshPrimitiveHelper& MeshPrimitiveHelper::effect(const Engine::EffectMaterialId& id)
{
    m_dto.effect(id);
    return *this;
}

MeshPrimitiveHelper& MeshPrimitiveHelper::textureMap(const Engine::EffectTextureMapDtoHelper& texture_map)
{
    m_dto.textureMap(texture_map);
    return *this;
}

MeshPrimitiveHelper& MeshPrimitiveHelper::renderListID(Renderer::Renderer::RenderListID id)
{
    m_dto.renderListID(id);
    return *this;
}

MeshPrimitiveHelper& MeshPrimitiveHelper::visualTechnique(const std::string& technique)
{
    m_dto.visualTechnique(technique);
    return *this;
}

MeshPrimitiveHelper& MeshPrimitiveHelper::asNative(const std::string& file_at_path)
{
    m_dto.asNative(file_at_path);
    return *this;
}

std::shared_ptr<MeshPrimitive> MeshPrimitiveHelper::constitute(Primitives::PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<MeshPrimitive>(std::make_shared<Primitives::RequestPrimitiveConstitution>(m_id, m_dto.toGenericDto(), persistence_level)->dispatch());
}

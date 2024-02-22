#include "RenderablePrimitiveHelper.h"
#include "Primitives/PrimitiveQueries.h"
#include "MeshPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "ModelPrimitive.h"

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

SkinMeshPrimitiveHelper::SkinMeshPrimitiveHelper(const Primitives::PrimitiveId& id) : m_id(id), m_dto(id)
{
}

SkinMeshPrimitiveHelper& SkinMeshPrimitiveHelper::geometryId(const Geometries::GeometryId& id)
{
    m_dto.geometryId(id);
    return *this;
}

SkinMeshPrimitiveHelper& SkinMeshPrimitiveHelper::effect(const Engine::EffectMaterialId& id)
{
    m_dto.effect(id);
    return *this;
}

SkinMeshPrimitiveHelper& SkinMeshPrimitiveHelper::textureMap(const Engine::EffectTextureMapDtoHelper& texture_map)
{
    m_dto.textureMap(texture_map);
    return *this;
}

SkinMeshPrimitiveHelper& SkinMeshPrimitiveHelper::renderListID(Renderer::Renderer::RenderListID id)
{
    m_dto.renderListID(id);
    return *this;
}

SkinMeshPrimitiveHelper& SkinMeshPrimitiveHelper::visualTechnique(const std::string& technique)
{
    m_dto.visualTechnique(technique);
    return *this;
}

SkinMeshPrimitiveHelper& SkinMeshPrimitiveHelper::asNative(const std::string& file_at_path)
{
    m_dto.asNative(file_at_path);
    return *this;
}

std::shared_ptr<SkinMeshPrimitive> SkinMeshPrimitiveHelper::constitute(Primitives::PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<SkinMeshPrimitive>(std::make_shared<Primitives::RequestPrimitiveConstitution>(m_id, m_dto.toGenericDto(), persistence_level)->dispatch());
}

ModelPrimitiveHelper::ModelPrimitiveHelper(const Primitives::PrimitiveId& id) : m_id(id), m_dto(id)
{
}

ModelPrimitiveHelper& ModelPrimitiveHelper::meshNodeTree(const MeshNodeTreeDtoHelper& tree)
{
    m_dto.meshNodeTree(tree);
    return *this;
}

ModelPrimitiveHelper& ModelPrimitiveHelper::animator(const Animators::AnimatorId& id)
{
    m_dto.animator(id);
    return *this;
}

ModelPrimitiveHelper& ModelPrimitiveHelper::asNative(const std::string& file_at_path)
{
    m_dto.asNative(file_at_path);
    return *this;
}

std::shared_ptr<ModelPrimitive> ModelPrimitiveHelper::constitute(Primitives::PersistenceLevel persistence_level)
{
    return std::dynamic_pointer_cast<ModelPrimitive>(std::make_shared<Primitives::RequestPrimitiveConstitution>(m_id, m_dto.toGenericDto(), persistence_level)->dispatch());
}

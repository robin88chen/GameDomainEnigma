/*********************************************************************
 * \file   RenderablePrimitiveHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef RENDERABLE_PRIMITIVE_HELPER_H
#define RENDERABLE_PRIMITIVE_HELPER_H

#include "RenderablePrimitiveDtoHelper.h"
#include "Primitives/PrimitivePersistenceLevel.h"

namespace Enigma::Renderables
{
    class MeshPrimitive;
    class SkinMeshPrimitive;
    class ModelPrimitive;

    class MeshPrimitiveHelper
    {
    public:
        MeshPrimitiveHelper(const Primitives::PrimitiveId& id);

        MeshPrimitiveHelper& geometryId(const Geometries::GeometryId& id);
        MeshPrimitiveHelper& effect(const Engine::EffectMaterialId& id);
        MeshPrimitiveHelper& textureMap(const Engine::EffectTextureMapDtoHelper& texture_map);
        MeshPrimitiveHelper& renderListID(Renderer::Renderer::RenderListID id);
        MeshPrimitiveHelper& visualTechnique(const std::string& technique);
        MeshPrimitiveHelper& asNative(const std::string& file_at_path);

        std::shared_ptr<MeshPrimitive> constitute(Primitives::PersistenceLevel persistence_level);

    protected:
        Primitives::PrimitiveId m_id;
        MeshPrimitiveDtoHelper m_dto;
    };

    class SkinMeshPrimitiveHelper
    {
    public:
        SkinMeshPrimitiveHelper(const Primitives::PrimitiveId& id);

        SkinMeshPrimitiveHelper& geometryId(const Geometries::GeometryId& id);
        SkinMeshPrimitiveHelper& effect(const Engine::EffectMaterialId& id);
        SkinMeshPrimitiveHelper& textureMap(const Engine::EffectTextureMapDtoHelper& texture_map);
        SkinMeshPrimitiveHelper& renderListID(Renderer::Renderer::RenderListID id);
        SkinMeshPrimitiveHelper& visualTechnique(const std::string& technique);
        SkinMeshPrimitiveHelper& asNative(const std::string& file_at_path);

        std::shared_ptr<SkinMeshPrimitive> constitute(Primitives::PersistenceLevel persistence_level);

    protected:
        Primitives::PrimitiveId m_id;
        SkinMeshPrimitiveDtoHelper m_dto;
    };

    class ModelPrimitiveHelper
    {
    public:
        ModelPrimitiveHelper(const Primitives::PrimitiveId& id);

        ModelPrimitiveHelper& asNative(const std::string& file_at_path);
        ModelPrimitiveHelper& meshNodeTree(const MeshNodeTreeDtoHelper& tree);
        ModelPrimitiveHelper& animator(const Animators::AnimatorId& id);

        std::shared_ptr<ModelPrimitive> constitute(Primitives::PersistenceLevel persistence_level);

    protected:
        Primitives::PrimitiveId m_id;
        ModelPrimitiveDtoHelper m_dto;
    };
}

#endif // RENDERABLE_PRIMITIVE_HELPER_H

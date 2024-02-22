/*********************************************************************
 * \file   RenderablePrimitiveDtoHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef RENDERABLE_PRIMITIVE_DTO_HELPER_H
#define RENDERABLE_PRIMITIVE_DTO_HELPER_H

#include "Primitives/PrimitiveId.h"
#include "RenderablePrimitiveDtos.h"
#include "Geometries/GeometryId.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/EffectDtoHelper.h"
#include "Animators/AnimatorId.h"

namespace Enigma::Renderables
{
    class MeshPrimitiveDtoHelper
    {
    public:
        MeshPrimitiveDtoHelper(const Primitives::PrimitiveId& id);

        MeshPrimitiveDtoHelper& geometryId(const Geometries::GeometryId& id);
        MeshPrimitiveDtoHelper& effect(const Engine::EffectMaterialId& id);
        MeshPrimitiveDtoHelper& textureMap(const Engine::EffectTextureMapDtoHelper& texture_map);
        MeshPrimitiveDtoHelper& renderListID(Renderer::Renderer::RenderListID id);
        MeshPrimitiveDtoHelper& visualTechnique(const std::string& technique);
        MeshPrimitiveDtoHelper& asNative(const std::string& file_at_path);

        Engine::GenericDto toGenericDto();

    protected:
        MeshPrimitiveDto m_dto;
    };

    class SkinMeshPrimitiveDtoHelper
    {
    public:
        SkinMeshPrimitiveDtoHelper(const Primitives::PrimitiveId& id);

        SkinMeshPrimitiveDtoHelper& geometryId(const Geometries::GeometryId& id);
        SkinMeshPrimitiveDtoHelper& effect(const Engine::EffectMaterialId& id);
        SkinMeshPrimitiveDtoHelper& textureMap(const Engine::EffectTextureMapDtoHelper& texture_map);
        SkinMeshPrimitiveDtoHelper& renderListID(Renderer::Renderer::RenderListID id);
        SkinMeshPrimitiveDtoHelper& visualTechnique(const std::string& technique);
        SkinMeshPrimitiveDtoHelper& asNative(const std::string& file_at_path);

        Engine::GenericDto toGenericDto();

    protected:
        SkinMeshPrimitiveDto m_dto;
    };

    class MeshNodeDtoHelper
    {
    public:
        MeshNodeDtoHelper(const std::string& name);

        MeshNodeDtoHelper& localT_PosTransform(MathLib::Matrix4& transform);
        MeshNodeDtoHelper& meshPrimitive(const Primitives::PrimitiveId& id);
        MeshNodeDtoHelper& parentNode(const std::string& name);

        Engine::GenericDto toGenericDto();

    protected:
        friend class MeshNodeTreeDtoHelper;
        MeshNodeDto m_dto;
        std::string m_name;
        std::string m_parentName;
    };

    class MeshNodeTreeDtoHelper
    {
    public:
        MeshNodeTreeDtoHelper();

        MeshNodeTreeDtoHelper& addNode(const MeshNodeDtoHelper& node);

        Engine::GenericDto toGenericDto();

    protected:
        std::optional<unsigned> findNodeIndex(const std::string& name);

    protected:
        MeshNodeTreeDto m_dto;
        std::vector<MeshNodeDtoHelper> m_nodes;
    };

    class ModelPrimitiveDtoHelper
    {
    public:
        ModelPrimitiveDtoHelper(const Primitives::PrimitiveId& id);

        ModelPrimitiveDtoHelper& asNative(const std::string& file_at_path);
        ModelPrimitiveDtoHelper& meshNodeTree(const MeshNodeTreeDtoHelper& tree);
        ModelPrimitiveDtoHelper& animator(const Animators::AnimatorId& id);

        Engine::GenericDto toGenericDto();

    protected:
        ModelPrimitiveDto m_dto;
    };
}

#endif // RENDERABLE_PRIMITIVE_DTO_HELPER_H

/*********************************************************************
 * \file   RenderablePrimitiveDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _RENDERABLE_PRIMITIVE_DTOS_H
#define _RENDERABLE_PRIMITIVE_DTOS_H

#include "GameEngine/GenericDto.h"
#include "MathLib/Matrix4.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameEngine/FactoryDesc.h"
#include "Renderer/Renderer.h"
#include "Primitives/PrimitiveId.h"
#include "Geometries/GeometryId.h"
#include "GameEngine/EffectMaterialId.h"
#include "Animators/AnimatorId.h"
#include <memory>
#include <vector>

namespace Enigma::Renderables
{
    class MeshPrimitive;
    class MeshPrimitiveDto
    {
    public:
        MeshPrimitiveDto();
        MeshPrimitiveDto(const Engine::GenericDto& dto);
        MeshPrimitiveDto(const MeshPrimitiveDto&) = default;
        MeshPrimitiveDto(MeshPrimitiveDto&&) = default;
        ~MeshPrimitiveDto() = default;
        MeshPrimitiveDto& operator=(const MeshPrimitiveDto&) = default;
        MeshPrimitiveDto& operator=(MeshPrimitiveDto&&) = default;

        [[nodiscard]] const Primitives::PrimitiveId& id() const { return m_id; }
        Primitives::PrimitiveId& id() { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const Geometries::GeometryId& geometryId() const { return m_geometryId; }
        Geometries::GeometryId& geometryId() { return m_geometryId; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& geometry() const { return m_geometry; }
        std::optional<Engine::GenericDto>& geometry() { return m_geometry; }
        [[nodiscard]] const std::vector<Engine::EffectMaterialId>& effects() const { return m_effects; }
        std::vector<Engine::EffectMaterialId>& effects() { return m_effects; }
        [[nodiscard]] const Engine::GenericDtoCollection& textureMaps() const { return m_textureMaps; }
        Engine::GenericDtoCollection& textureMaps() { return m_textureMaps; }
        [[nodiscard]] Renderer::Renderer::RenderListID renderListID() const { return m_renderListID; }
        Renderer::Renderer::RenderListID& renderListID() { return m_renderListID; }
        [[nodiscard]] const std::string& visualTechniqueSelection() const { return m_visualTechniqueSelection; }
        std::string& visualTechniqueSelection() { return m_visualTechniqueSelection; }

        Engine::GenericDto toGenericDto() const;

    protected:
        Primitives::PrimitiveId m_id;
        Engine::FactoryDesc m_factoryDesc;
        Geometries::GeometryId m_geometryId;
        std::optional<Engine::GenericDto> m_geometry;
        std::vector<Engine::EffectMaterialId> m_effects;
        Engine::GenericDtoCollection m_textureMaps;
        Renderer::Renderer::RenderListID m_renderListID;
        std::string m_visualTechniqueSelection;
    };

    class MeshPrimitiveMetaDto
    {
    public:
        MeshPrimitiveMetaDto(const MeshPrimitiveDto& dto);

        void replaceDuplicatedEffects(const std::shared_ptr<MeshPrimitive>& mesh_primitive);

        [[nodiscard]] const std::vector<Engine::EffectMaterialId>& effects() const { return m_effects; }

        [[nodiscard]] const std::vector<Engine::EffectTextureMapDto>& textureMaps() const { return m_textureMaps; }

    protected:
        std::vector<Engine::EffectMaterialId> m_effects;
        std::vector<Engine::EffectTextureMapDto> m_textureMaps;
    };

    class SkinMeshPrimitiveDto : public MeshPrimitiveDto
    {
    public:
        SkinMeshPrimitiveDto();
        SkinMeshPrimitiveDto(const Engine::GenericDto& dto);
        SkinMeshPrimitiveDto(const MeshPrimitiveDto&);
        SkinMeshPrimitiveDto(const SkinMeshPrimitiveDto&) = default;
        SkinMeshPrimitiveDto(SkinMeshPrimitiveDto&&) = default;
        ~SkinMeshPrimitiveDto() = default;
        SkinMeshPrimitiveDto& operator=(const SkinMeshPrimitiveDto&) = default;
        SkinMeshPrimitiveDto& operator=(SkinMeshPrimitiveDto&&) = default;

        Engine::GenericDto toGenericDto() const;
    };

    class MeshNodeDto
    {
    public:
        MeshNodeDto();
        MeshNodeDto(const Engine::GenericDto& dto);
        MeshNodeDto(const MeshNodeDto&) = default;
        MeshNodeDto(MeshNodeDto&&) = default;
        ~MeshNodeDto() = default;
        MeshNodeDto& operator=(const MeshNodeDto&) = default;
        MeshNodeDto& operator=(MeshNodeDto&&) = default;

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const std::string& name() const { return m_name; }
        std::string& name() { return m_name; }
        [[nodiscard]] const MathLib::Matrix4& localT_PosTransform() const { return m_localT_PosTransform; }
        MathLib::Matrix4& localT_PosTransform() { return m_localT_PosTransform; }
        //[[nodiscard]] const MathLib::Matrix4& RootRefTransform() const { return m_rootRefTransform; }
        //MathLib::Matrix4& RootRefTransform() { return m_rootRefTransform; }
        [[nodiscard]] const std::optional<Primitives::PrimitiveId>& meshPrimitiveId() const { return m_meshPrimitiveId; }
        std::optional<Primitives::PrimitiveId>& meshPrimitiveId() { return m_meshPrimitiveId; }
        [[nodiscard]] std::optional<unsigned> parentIndexInArray() const { return m_parentIndexInArray; }
        std::optional<unsigned>& parentIndexInArray() { return m_parentIndexInArray; }

        Engine::GenericDto toGenericDto() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::string m_name;
        MathLib::Matrix4 m_localT_PosTransform;
        //MathLib::Matrix4 m_rootRefTransform;
        std::optional<Primitives::PrimitiveId> m_meshPrimitiveId;
        std::optional<unsigned> m_parentIndexInArray;
    };

    class MeshNodeTreeDto
    {
    public:
        MeshNodeTreeDto();
        MeshNodeTreeDto(const Engine::GenericDto& dto);
        MeshNodeTreeDto(const MeshNodeTreeDto&) = default;
        MeshNodeTreeDto(MeshNodeTreeDto&&) = default;
        ~MeshNodeTreeDto() = default;
        MeshNodeTreeDto& operator=(const MeshNodeTreeDto&) = default;
        MeshNodeTreeDto& operator=(MeshNodeTreeDto&&) = default;

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const Engine::GenericDtoCollection& meshNodes() const { return m_nodeDtos; }
        Engine::GenericDtoCollection& meshNodes() { return m_nodeDtos; }

        Engine::GenericDto toGenericDto() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        Engine::GenericDtoCollection m_nodeDtos;
    };

    class ModelPrimitiveDto
    {
    public:
        ModelPrimitiveDto();
        ModelPrimitiveDto(const Engine::GenericDto& dto);
        ModelPrimitiveDto(const ModelPrimitiveDto&) = default;
        ModelPrimitiveDto(ModelPrimitiveDto&&) = default;
        ~ModelPrimitiveDto() = default;
        ModelPrimitiveDto& operator=(const ModelPrimitiveDto&) = default;
        ModelPrimitiveDto& operator=(ModelPrimitiveDto&&) = default;

        [[nodiscard]] const Primitives::PrimitiveId& id() const { return m_id; }
        Primitives::PrimitiveId& id() { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const Engine::GenericDto& nodeTree() const { return m_nodeTreeDto; }
        Engine::GenericDto& nodeTree() { return m_nodeTreeDto; }
        [[nodiscard]] const std::optional<Animators::AnimatorId>& animatorId() const { return m_animatorId; }
        std::optional<Animators::AnimatorId>& animatorId() { return m_animatorId; }

        Engine::GenericDto toGenericDto() const;

    protected:
        Primitives::PrimitiveId m_id;
        Engine::FactoryDesc m_factoryDesc;
        Engine::GenericDto m_nodeTreeDto;
        std::optional<Animators::AnimatorId> m_animatorId;
    };
}

#endif // _RENDERABLE_PRIMITIVE_DTOS_H

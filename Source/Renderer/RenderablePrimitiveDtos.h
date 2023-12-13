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
#include "Geometries/GeometryDataDto.h"
#include "GameEngine/FactoryDesc.h"
#include "GameEngine/DtoDeserializer.h"
#include "Renderer.h"
#include "GameEngine/PrimitiveId.h"
#include "Geometries/GeometryId.h"
#include "GameEngine/EffectMaterialDto.h"
#include <memory>
#include <vector>

namespace Enigma::Renderer
{
    class MeshPrimitivePolicy;
    class ModelPrimitivePolicy;
    class SkinMeshPrimitivePolicy;

    class MeshPrimitiveDto
    {
    public:
        MeshPrimitiveDto();
        MeshPrimitiveDto(const MeshPrimitiveDto&) = default;
        MeshPrimitiveDto(MeshPrimitiveDto&&) = default;
        ~MeshPrimitiveDto() = default;
        MeshPrimitiveDto& operator=(const MeshPrimitiveDto&) = default;
        MeshPrimitiveDto& operator=(MeshPrimitiveDto&&) = default;

        [[nodiscard]] const Engine::PrimitiveId& id() const { return m_id; }
        Engine::PrimitiveId& id() { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const std::string& name() const { return m_name; }
        std::string& name() { return m_name; }
        [[nodiscard]] const Geometries::GeometryId& geometryId() const { return m_geometryId; }
        Geometries::GeometryId& geometryId() { return m_geometryId; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& geometry() const { return m_geometry; }
        std::optional<Engine::GenericDto>& geometry() { return m_geometry; }
        //[[nodiscard]] const Engine::FactoryDesc& geometryFactoryDesc() const { return m_geometryFactory; }
        //Engine::FactoryDesc& geometryFactoryDesc() { return m_geometryFactory; }
        [[nodiscard]] const Engine::GenericDtoCollection& effects() const { return m_effects; }
        Engine::GenericDtoCollection& effects() { return m_effects; }
        [[nodiscard]] const Engine::GenericDtoCollection& textureMaps() const { return m_textureMaps; }
        Engine::GenericDtoCollection& textureMaps() { return m_textureMaps; }
        [[nodiscard]] Renderer::RenderListID renderListID() const { return m_renderListID; }
        Renderer::RenderListID& renderListID() { return m_renderListID; }
        [[nodiscard]] const std::string& visualTechniqueSelection() const { return m_visualTechniqueSelection; }
        std::string& visualTechniqueSelection() { return m_visualTechniqueSelection; }

        static MeshPrimitiveDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

        //static std::shared_ptr<Engine::GenericPolicy> meshDtoConvertToPolicy(const Engine::GenericDto&, const std::shared_ptr<Engine::IDtoDeserializer>&);

    protected:
        Engine::PrimitiveId m_id;
        Engine::FactoryDesc m_factoryDesc;
        std::string m_name;
        Geometries::GeometryId m_geometryId;
        std::optional<Engine::GenericDto> m_geometry;
        //Engine::FactoryDesc m_geometryFactory;
        Engine::GenericDtoCollection m_effects;
        Engine::GenericDtoCollection m_textureMaps;
        Renderer::RenderListID m_renderListID;
        std::string m_visualTechniqueSelection;
    };

    class MeshPrimitiveMetaDto
    {
    public:
        MeshPrimitiveMetaDto(const MeshPrimitiveDto& dto);

        [[nodiscard]] const std::vector<Engine::EffectMaterialDto>& effects() const { return m_effects; }

        [[nodiscard]] const std::vector<Engine::EffectTextureMapDto>& textureMaps() const { return m_textureMaps; }

    protected:
        std::vector<Engine::EffectMaterialDto> m_effects;
        std::vector<Engine::EffectTextureMapDto> m_textureMaps;
    };

    class SkinMeshPrimitiveDto : public MeshPrimitiveDto
    {
    public:
        SkinMeshPrimitiveDto();
        SkinMeshPrimitiveDto(const MeshPrimitiveDto&);
        SkinMeshPrimitiveDto(const SkinMeshPrimitiveDto&) = default;
        SkinMeshPrimitiveDto(SkinMeshPrimitiveDto&&) = default;
        ~SkinMeshPrimitiveDto() = default;
        SkinMeshPrimitiveDto& operator=(const SkinMeshPrimitiveDto&) = default;
        SkinMeshPrimitiveDto& operator=(SkinMeshPrimitiveDto&&) = default;

        static SkinMeshPrimitiveDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;
        static std::shared_ptr<Engine::GenericPolicy> skinMeshDtoConvertToPolicy(const Engine::GenericDto&, const std::shared_ptr<Engine::IDtoDeserializer>&);
    };

    class MeshNodeDto
    {
    public:
        MeshNodeDto();
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
        [[nodiscard]] const std::optional<Engine::GenericDto>& meshPrimitive() const { return m_meshPrimitive; }
        std::optional<Engine::GenericDto>& meshPrimitive() { return m_meshPrimitive; }
        [[nodiscard]] std::optional<unsigned> parentIndexInArray() const { return m_parentIndexInArray; }
        std::optional<unsigned>& parentIndexInArray() { return m_parentIndexInArray; }

        static MeshNodeDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::string m_name;
        MathLib::Matrix4 m_localT_PosTransform;
        //MathLib::Matrix4 m_rootRefTransform;
        std::optional<Engine::GenericDto> m_meshPrimitive;
        std::optional<unsigned> m_parentIndexInArray;
    };

    class MeshNodeTreeDto
    {
    public:
        MeshNodeTreeDto();
        MeshNodeTreeDto(const MeshNodeTreeDto&) = default;
        MeshNodeTreeDto(MeshNodeTreeDto&&) = default;
        ~MeshNodeTreeDto() = default;
        MeshNodeTreeDto& operator=(const MeshNodeTreeDto&) = default;
        MeshNodeTreeDto& operator=(MeshNodeTreeDto&&) = default;

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const Engine::GenericDtoCollection& meshNodes() const { return m_nodeDtos; }
        Engine::GenericDtoCollection& meshNodes() { return m_nodeDtos; }

        static MeshNodeTreeDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
        Engine::GenericDtoCollection m_nodeDtos;
    };

    class ModelPrimitiveDto
    {
    public:
        ModelPrimitiveDto();
        ModelPrimitiveDto(const ModelPrimitiveDto&) = default;
        ModelPrimitiveDto(ModelPrimitiveDto&&) = default;
        ~ModelPrimitiveDto() = default;
        ModelPrimitiveDto& operator=(const ModelPrimitiveDto&) = default;
        ModelPrimitiveDto& operator=(ModelPrimitiveDto&&) = default;

        [[nodiscard]] const Engine::PrimitiveId& id() const { return m_id; }
        Engine::PrimitiveId& id() { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const std::string& name() const { return m_name; }
        std::string& name() { return m_name; }
        [[nodiscard]] const Engine::GenericDto& nodeTree() const { return m_nodeTreeDto; }
        Engine::GenericDto& nodeTree() { return m_nodeTreeDto; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& animator() const { return m_animatorDto; }
        std::optional<Engine::GenericDto>& animator() { return m_animatorDto; }

        static ModelPrimitiveDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

        static std::shared_ptr<Engine::GenericPolicy> modelDtoConvertToPolicy(const Engine::GenericDto&, const std::shared_ptr<Engine::IDtoDeserializer>&);

    protected:
        Engine::PrimitiveId m_id;
        Engine::FactoryDesc m_factoryDesc;
        std::string m_name;
        Engine::GenericDto m_nodeTreeDto;
        std::optional<Engine::GenericDto> m_animatorDto;
    };
}

#endif // _RENDERABLE_PRIMITIVE_DTOS_H

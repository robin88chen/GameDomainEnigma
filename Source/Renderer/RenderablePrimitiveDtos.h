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
#include "GameEngine/GeometryDataDto.h"
#include "GameEngine/FactoryDesc.h"
#include "GameEngine/DtoDeserializer.h"
#include "Renderer.h"
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

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] const std::string& GeometryName() const { return m_geometryName; }
        std::string& GeometryName() { return m_geometryName; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& TheGeometry() const { return m_geometry; }
        std::optional<Engine::GenericDto>& TheGeometry() { return m_geometry; }
        [[nodiscard]] const Engine::FactoryDesc& GeometryFactoryDesc() const { return m_geometryFactory; }
        Engine::FactoryDesc& GeometryFactoryDesc() { return m_geometryFactory; }
        [[nodiscard]] const Engine::GenericDtoCollection& Effects() const { return m_effects; }
        Engine::GenericDtoCollection& Effects() { return m_effects; }
        [[nodiscard]] const Engine::GenericDtoCollection& TextureMaps() const { return m_textureMaps; }
        Engine::GenericDtoCollection& TextureMaps() { return m_textureMaps; }
        [[nodiscard]] Renderer::RenderListID RenderListID() const { return m_renderListID; }
        Renderer::RenderListID& RenderListID() { return m_renderListID; }

        static MeshPrimitiveDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

        static std::shared_ptr<Engine::GenericPolicy> MeshDtoConvertToPolicy(const Engine::GenericDto&, const std::shared_ptr<Engine::IDtoDeserializer>&);

    protected:
        std::string m_name;
        std::string m_geometryName;
        std::optional<Engine::GenericDto> m_geometry;
        Engine::FactoryDesc m_geometryFactory;
        Engine::GenericDtoCollection m_effects;
        Engine::GenericDtoCollection m_textureMaps;
        Renderer::RenderListID m_renderListID;
    };

    class SkinMeshPrimitiveDto : public MeshPrimitiveDto
    {
    public:
        SkinMeshPrimitiveDto() : MeshPrimitiveDto() {};
        SkinMeshPrimitiveDto(const MeshPrimitiveDto&);
        SkinMeshPrimitiveDto(const SkinMeshPrimitiveDto&) = default;
        SkinMeshPrimitiveDto(SkinMeshPrimitiveDto&&) = default;
        ~SkinMeshPrimitiveDto() = default;
        SkinMeshPrimitiveDto& operator=(const SkinMeshPrimitiveDto&) = default;
        SkinMeshPrimitiveDto& operator=(SkinMeshPrimitiveDto&&) = default;

        static SkinMeshPrimitiveDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;
        static std::shared_ptr<Engine::GenericPolicy> SkinMeshDtoConvertToPolicy(const Engine::GenericDto&, const std::shared_ptr<Engine::IDtoDeserializer>&);
    };

    class MeshNodeDto
    {
    public:
        MeshNodeDto() = default;
        MeshNodeDto(const MeshNodeDto&) = default;
        MeshNodeDto(MeshNodeDto&&) = default;
        ~MeshNodeDto() = default;
        MeshNodeDto& operator=(const MeshNodeDto&) = default;
        MeshNodeDto& operator=(MeshNodeDto&&) = default;

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] const MathLib::Matrix4& LocalT_PosTransform() const { return m_localT_PosTransform; }
        MathLib::Matrix4& LocalT_PosTransform() { return m_localT_PosTransform; }
        //[[nodiscard]] const MathLib::Matrix4& RootRefTransform() const { return m_rootRefTransform; }
        //MathLib::Matrix4& RootRefTransform() { return m_rootRefTransform; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& TheMeshPrimitive() const { return m_meshPrimitive; }
        std::optional<Engine::GenericDto>& TheMeshPrimitive() { return m_meshPrimitive; }
        [[nodiscard]] std::optional<unsigned> ParentIndexInArray() const { return m_parentIndexInArray; }
        std::optional<unsigned>& ParentIndexInArray() { return m_parentIndexInArray; }

        static MeshNodeDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

    protected:
        std::string m_name;
        MathLib::Matrix4 m_localT_PosTransform;
        //MathLib::Matrix4 m_rootRefTransform;
        std::optional<Engine::GenericDto> m_meshPrimitive;
        std::optional<unsigned> m_parentIndexInArray;
    };

    class MeshNodeTreeDto
    {
    public:
        MeshNodeTreeDto() = default;
        MeshNodeTreeDto(const MeshNodeTreeDto&) = default;
        MeshNodeTreeDto(MeshNodeTreeDto&&) = default;
        ~MeshNodeTreeDto() = default;
        MeshNodeTreeDto& operator=(const MeshNodeTreeDto&) = default;
        MeshNodeTreeDto& operator=(MeshNodeTreeDto&&) = default;

        [[nodiscard]] const Engine::GenericDtoCollection& MeshNodes() const { return m_nodeDtos; }
        Engine::GenericDtoCollection& MeshNodes() { return m_nodeDtos; }

        static MeshNodeTreeDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

    protected:
        Engine::GenericDtoCollection m_nodeDtos;
    };

    class ModelPrimitiveDto
    {
    public:
        ModelPrimitiveDto() = default;
        ModelPrimitiveDto(const ModelPrimitiveDto&) = default;
        ModelPrimitiveDto(ModelPrimitiveDto&&) = default;
        ~ModelPrimitiveDto() = default;
        ModelPrimitiveDto& operator=(const ModelPrimitiveDto&) = default;
        ModelPrimitiveDto& operator=(ModelPrimitiveDto&&) = default;

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] const Engine::GenericDto& TheNodeTree() const { return m_nodeTreeDto; }
        Engine::GenericDto& TheNodeTree() { return m_nodeTreeDto; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& TheAnimator() const { return m_animatorDto; }
        std::optional<Engine::GenericDto>& TheAnimator() { return m_animatorDto; }

        static ModelPrimitiveDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

        static std::shared_ptr<Engine::GenericPolicy> ModelDtoConvertToPolicy(const Engine::GenericDto&, const std::shared_ptr<Engine::IDtoDeserializer>&);

    protected:
        std::string m_name;
        Engine::GenericDto m_nodeTreeDto;
        std::optional<Engine::GenericDto> m_animatorDto;
    };
}

#endif // _RENDERABLE_PRIMITIVE_DTOS_H

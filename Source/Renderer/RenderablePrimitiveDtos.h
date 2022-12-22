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
#include "GameEngine/EffectMaterialDto.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameEngine/GeometryDataDto.h"
#include "GameEngine/FactoryDesc.h"
#include "GameEngine/DtoDeserializer.h"
#include <memory>
#include <vector>

namespace Enigma::Renderer
{
    class MeshPrimitivePolicy;

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
        [[nodiscard]] const std::vector<Engine::GenericDto>& Effects() const { return m_effects; }
        std::vector<Engine::GenericDto>& Effects() { return m_effects; }
        [[nodiscard]] const std::vector<Engine::GenericDto>& TextureMaps() const { return m_textureMaps; }
        std::vector<Engine::GenericDto>& TextureMaps() { return m_textureMaps; }

        static MeshPrimitiveDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto();

        std::shared_ptr<MeshPrimitivePolicy> ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>&,
            const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>&);

    protected:
        std::string m_name;
        std::string m_geometryName;
        std::optional<Engine::GenericDto> m_geometry;
        Engine::FactoryDesc m_geometryFactory;
        std::vector<Engine::GenericDto> m_effects;
        std::vector<Engine::GenericDto> m_textureMaps;
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
        [[nodiscard]] const MathLib::Matrix4& LocalTransform() const { return m_localTransform; }
        MathLib::Matrix4& LocalTransform() { return m_localTransform; }
        [[nodiscard]] const MathLib::Matrix4& RootRefTransform() const { return m_rootRefTransform; }
        MathLib::Matrix4& RootRefTransform() { return m_rootRefTransform; }
        [[nodiscard]] const std::optional<MeshPrimitiveDto>& TheMeshPrimitive() const { return m_meshPrimitive; }
        std::optional<MeshPrimitiveDto>& TheMeshPrimitive() { return m_meshPrimitive; }
        [[nodiscard]] std::optional<unsigned> ParentIndexInArray() const { return m_parentIndexInArray; }
        std::optional<unsigned>& ParentIndexInArray() { return m_parentIndexInArray; }

    protected:
        std::string m_name;
        MathLib::Matrix4 m_localTransform;
        MathLib::Matrix4 m_rootRefTransform;
        std::optional<MeshPrimitiveDto> m_meshPrimitive;
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

        [[nodiscard]] const std::vector<MeshNodeDto>& MeshNodes() const { return m_nodeDtos; }
        std::vector<MeshNodeDto>& MeshNodes() { return m_nodeDtos; }

    protected:
        std::vector<MeshNodeDto> m_nodeDtos;
    };
}

#endif // _RENDERABLE_PRIMITIVE_DTOS_H

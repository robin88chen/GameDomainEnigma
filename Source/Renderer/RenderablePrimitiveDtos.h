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
#include <memory>
#include <vector>

namespace Enigma::Renderer
{
    class MeshPrimitivePolicy;

    class MeshNodeDto
    {
    public:
        MeshNodeDto() = default;
        MeshNodeDto(const MeshNodeDto&) = delete;
        MeshNodeDto(MeshNodeDto&&) = default;
        ~MeshNodeDto() = default;

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] const MathLib::Matrix4& LocalTransform() const { return m_localTransform; }
        MathLib::Matrix4& LocalTransform() { return m_localTransform; }
        [[nodiscard]] const MathLib::Matrix4& RootRefTransform() const { return m_rootRefTransform; }
        MathLib::Matrix4& RootRefTransform() { return m_rootRefTransform; }
        [[nodiscard]] std::unique_ptr<MeshPrimitivePolicy>& GetMeshPrimitivePolicy() { return m_meshPrimitive; }
        void SetMeshPrimitivePolicy(std::unique_ptr<MeshPrimitivePolicy> policy);
        [[nodiscard]] std::optional<unsigned> ParentIndexInArray() const { return m_parentIndexInArray; }
        std::optional<unsigned>& ParentIndexInArray() { return m_parentIndexInArray; }

    protected:
        std::string m_name;
        MathLib::Matrix4 m_localTransform;
        MathLib::Matrix4 m_rootRefTransform;
        std::unique_ptr<MeshPrimitivePolicy> m_meshPrimitive;
        std::optional<unsigned> m_parentIndexInArray;
    };

    class MeshNodeTreeDto
    {
    public:
        MeshNodeTreeDto() = default;
        MeshNodeTreeDto(const MeshNodeDto&) = delete;
        MeshNodeTreeDto(MeshNodeTreeDto&&) = default;
        ~MeshNodeTreeDto() = default;

        [[nodiscard]] const std::vector<MeshNodeDto>& MeshNodes() const { return m_nodeDtos; }
        std::vector<MeshNodeDto>& MeshNodes() { return m_nodeDtos; }

    protected:
        std::vector<MeshNodeDto> m_nodeDtos;
    };
}

#endif // _RENDERABLE_PRIMITIVE_DTOS_H

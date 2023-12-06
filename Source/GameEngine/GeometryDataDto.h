/*********************************************************************
 * \file   GeometryDataDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_DATA_DTO_H
#define _GEOMETRY_DATA_DTO_H

#include <string>
#include <vector>
#include <optional>
#include "MathLib/Vector3.h"
#include "MathLib/Vector4.h"
#include "MathLib/Vector2.h"
#include "GenericDto.h"
#include "GeometryId.h"

namespace Enigma::Engine
{
    class TextureCoordDto
    {
    public:
        TextureCoordDto() = default;

        static TextureCoordDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto();

        [[nodiscard]] std::optional<std::vector<MathLib::Vector2>> texture2DCoords() const { return m_2dCoords; }
        std::optional<std::vector<MathLib::Vector2>>& texture2DCoords() { return m_2dCoords; }
        [[nodiscard]] std::optional<std::vector<float>> texture1DCoords() const { return m_1dCoords; }
        std::optional<std::vector<float>>& texture1DCoords() { return m_1dCoords; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> texture3DCoords() const { return m_3dCoords; }
        std::optional<std::vector<MathLib::Vector3>>& texture3DCoords() { return m_3dCoords; }

    protected:
        std::optional<std::vector<MathLib::Vector2>> m_2dCoords;
        std::optional<std::vector<float>> m_1dCoords;
        std::optional<std::vector<MathLib::Vector3>> m_3dCoords;
    };

    class GeometryDataDto
    {
    public:
        GeometryDataDto();

        static GeometryDataDto fromGenericDto(const GenericDto& dto);
        GenericDto toGenericDto() const;
        void deserializeNonVertexAttributesFromGenericDto(const GenericDto& dto);
        void serializeNonVertexAttributesToGenericDto(GenericDto& dto) const;

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const GeometryId& id() const { return m_id; }
        GeometryId& id() { return m_id; }
        [[nodiscard]] const std::string& vertexFormat() const { return m_vertexFormat; }
        std::string& vertexFormat() { return m_vertexFormat; }
        [[nodiscard]] const std::vector<unsigned>& segments() const { return m_segments; }
        std::vector<unsigned>& segments() { return m_segments; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> position3s() const { return m_position3s; }
        std::optional<std::vector<MathLib::Vector3>>& position3s() { return m_position3s; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector4>> position4s() const { return m_position4s; }
        std::optional<std::vector<MathLib::Vector4>>& position4s() { return m_position4s; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> normals() const { return m_normals; }
        std::optional<std::vector<MathLib::Vector3>>& normals() { return m_normals; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector4>> diffuseColors() const { return m_diffuseColors; }
        std::optional<std::vector<MathLib::Vector4>>& diffuseColors() { return m_diffuseColors; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector4>> specularColors() const { return m_specularColors; }
        std::optional<std::vector<MathLib::Vector4>>& specularColors() { return m_specularColors; }
        [[nodiscard]] const GenericDtoCollection& textureCoords() const { return m_texCoords; }
        GenericDtoCollection& textureCoords() { return m_texCoords; }
        [[nodiscard]] std::optional<std::vector<unsigned>> paletteIndices() const { return m_paletteIndices; }
        std::optional<std::vector<unsigned>>& paletteIndices() { return m_paletteIndices; }
        [[nodiscard]] std::optional<std::vector<float>> weights() const { return m_weights; }
        std::optional<std::vector<float>>& weights() { return m_weights; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector4>> tangents() const { return m_tangents; }
        std::optional<std::vector<MathLib::Vector4>>& tangents() { return m_tangents; }
        [[nodiscard]] std::optional<std::vector<unsigned>> indices() const { return m_indices; }
        std::optional<std::vector<unsigned>>& indices() { return m_indices; }
        [[nodiscard]] unsigned vertexCapacity() const { return m_vtxCapacity; }
        unsigned& vertexCapacity() { return m_vtxCapacity; }
        [[nodiscard]] unsigned indexCapacity() const { return m_idxCapacity; }
        unsigned& indexCapacity() { return m_idxCapacity; }
        [[nodiscard]] unsigned vertexUsedCount() const { return m_vtxUsedCount; }
        unsigned& vertexUsedCount() { return m_vtxUsedCount; }
        [[nodiscard]] unsigned indexUsedCount() const { return m_idxUsedCount; }
        unsigned& indexUsedCount() { return m_idxUsedCount; }
        [[nodiscard]] unsigned topology() const { return m_topology; }
        unsigned& topology() { return m_topology; }
        [[nodiscard]] const GenericDto& geometryBound() const { return m_geometryBound; }
        GenericDto& geometryBound() { return m_geometryBound; }

    protected:
        FactoryDesc m_factoryDesc;
        GeometryId m_id;
        std::string m_vertexFormat;
        std::vector<unsigned> m_segments;
        std::optional<std::vector<MathLib::Vector3>> m_position3s;
        std::optional<std::vector<MathLib::Vector4>> m_position4s;
        std::optional<std::vector<MathLib::Vector3>> m_normals;
        std::optional<std::vector<MathLib::Vector4>> m_diffuseColors;
        std::optional<std::vector<MathLib::Vector4>> m_specularColors;
        GenericDtoCollection m_texCoords;
        std::optional<std::vector<unsigned>> m_paletteIndices;
        std::optional<std::vector<float>> m_weights;
        std::optional<std::vector<MathLib::Vector4>> m_tangents;
        std::optional<std::vector<unsigned>> m_indices;
        unsigned m_vtxCapacity;
        unsigned m_idxCapacity;
        unsigned m_vtxUsedCount;
        unsigned m_idxUsedCount;
        unsigned m_topology;
        GenericDto m_geometryBound;
    };

    class TriangleListDto : public GeometryDataDto
    {
    public:
        TriangleListDto();
        TriangleListDto(const GeometryDataDto& geometry_dto);

        static TriangleListDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;
    };
}

#endif // _GEOMETRY_DATA_DTO_H

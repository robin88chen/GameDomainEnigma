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

namespace Enigma::Engine
{
    class TextureCoordDto
    {
    public:
        TextureCoordDto() = default;

        static TextureCoordDto FromGenericDto(const GenericDto& dto);
        GenericDto ToGenericDto();

        [[nodiscard]] std::optional<std::vector<MathLib::Vector2>> Texture2DCoords() const { return m_2dCoords; }
        std::optional<std::vector<MathLib::Vector2>>& Texture2DCoords() { return m_2dCoords; }
        [[nodiscard]] std::optional<std::vector<float>> Texture1DCoords() const { return m_1dCoords; }
        std::optional<std::vector<float>>& Texture1DCoords() { return m_1dCoords; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> Texture3DCoords() const { return m_3dCoords; }
        std::optional<std::vector<MathLib::Vector3>>& Texture3DCoords() { return m_3dCoords; }

    protected:
        std::optional<std::vector<MathLib::Vector2>> m_2dCoords;
        std::optional<std::vector<float>> m_1dCoords;
        std::optional<std::vector<MathLib::Vector3>> m_3dCoords;
    };

    class GeometryDataDto
    {
    public:
        GeometryDataDto() : m_vtxCapacity(0), m_idxCapacity(0), m_vtxUsedCount(0), m_idxUsedCount(0), m_topology(0) {};

        static GeometryDataDto FromGenericDto(const GenericDto& dto);
        GenericDto ToGenericDto();

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }
        [[nodiscard]] const std::string& VertexFormat() const { return m_vertexFormat; }
        std::string& VertexFormat() { return m_vertexFormat; }
        [[nodiscard]] const std::vector<unsigned>& Segments() const { return m_segments; }
        std::vector<unsigned>& Segments() { return m_segments; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> Position3s() const { return m_position3s; }
        std::optional<std::vector<MathLib::Vector3>>& Position3s() { return m_position3s; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector4>> Position4s() const { return m_position4s; }
        std::optional<std::vector<MathLib::Vector4>>& Position4s() { return m_position4s; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> Normals() const { return m_normals; }
        std::optional<std::vector<MathLib::Vector3>>& Normals() { return m_normals; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector4>> DiffuseColors() const { return m_diffuseColors; }
        std::optional<std::vector<MathLib::Vector4>>& DiffuseColors() { return m_diffuseColors; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector4>> SpecularColors() const { return m_specularColors; }
        std::optional<std::vector<MathLib::Vector4>>& SpecularColors() { return m_specularColors; }
        [[nodiscard]] const std::vector<GenericDto>& TextureCoords() const { return m_texCoords; }
        std::vector<GenericDto>& TextureCoords() { return m_texCoords; }
        [[nodiscard]] std::optional<std::vector<unsigned>> PaletteIndices() const { return m_paletteIndices; }
        std::optional<std::vector<unsigned>>& PaletteIndices() { return m_paletteIndices; }
        [[nodiscard]] std::optional<std::vector<float>> Weights() const { return m_weights; }
        std::optional<std::vector<float>>& Weights() { return m_weights; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector4>> Tangents() const { return m_tangents; }
        std::optional<std::vector<MathLib::Vector4>>& Tangents() { return m_tangents; }
        [[nodiscard]] std::optional<std::vector<unsigned>> Indices() const { return m_indices; }
        std::optional<std::vector<unsigned>>& Indices() { return m_indices; }
        [[nodiscard]] unsigned VertexCapacity() const { return m_vtxCapacity; }
        unsigned& VertexCapacity() { return m_vtxCapacity; }
        [[nodiscard]] unsigned IndexCapacity() const { return m_idxCapacity; }
        unsigned& IndexCapacity() { return m_idxCapacity; }
        [[nodiscard]] unsigned VertexUsedCount() const { return m_vtxUsedCount; }
        unsigned& VertexUsedCount() { return m_vtxUsedCount; }
        [[nodiscard]] unsigned IndexUsedCount() const { return m_idxUsedCount; }
        unsigned& IndexUsedCount() { return m_idxUsedCount; }
        [[nodiscard]] unsigned Topology() const { return m_topology; }
        unsigned& Topology() { return m_topology; }
        [[nodiscard]] const GenericDto& GeometryBound() const { return m_geometryBound; }
        GenericDto& GeometryBound() { return m_geometryBound; }

    protected:
        std::string m_name;
        std::string m_vertexFormat;
        std::vector<unsigned> m_segments;
        std::optional<std::vector<MathLib::Vector3>> m_position3s;
        std::optional<std::vector<MathLib::Vector4>> m_position4s;
        std::optional<std::vector<MathLib::Vector3>> m_normals;
        std::optional<std::vector<MathLib::Vector4>> m_diffuseColors;
        std::optional<std::vector<MathLib::Vector4>> m_specularColors;
        std::vector<GenericDto> m_texCoords;
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
        TriangleListDto() = default;
        TriangleListDto(const GeometryDataDto& geometry_dto);

        static TriangleListDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto();

    protected:
        std::vector<std::string> m_childNames;
    };
}

#endif // _GEOMETRY_DATA_DTO_H

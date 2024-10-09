/*********************************************************************
 * \file   GeometryAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef GEOMETRY_ASSEMBLER_H
#define GEOMETRY_ASSEMBLER_H

#include "GeometryId.h"
#include "GameEngine/GenericDto.h"
#include "GameEngine/FactoryDesc.h"
#include "MathLib/Vector3.h"
#include "MathLib/Vector4.h"
#include "GeometrySegment.h"
#include "GraphicKernel/GraphicAPITypes.h"
#include "GameEngine/BoundingVolume.h"
#include "TextureCoordinateAssembler.h"
#include "GraphicKernel/VertexFormatCode.h"
#include <optional>
#include <vector>
#include <string>

namespace Enigma::Geometries
{
    class GeometryAssembler
    {
    public:
        GeometryAssembler(const GeometryId& id);
        GeometryAssembler(const GeometryId& id, const Engine::FactoryDesc& desc);
        virtual ~GeometryAssembler() = default;
        GeometryAssembler(const GeometryAssembler&) = default;
        GeometryAssembler(GeometryAssembler&&) = default;
        GeometryAssembler& operator=(const GeometryAssembler&) = default;
        GeometryAssembler& operator=(GeometryAssembler&&) = default;

        virtual Engine::GenericDto assemble();

        void asAsset(const std::string& name, const std::string& filename, const std::string& path_id);
        void factoryDesc(const Engine::FactoryDesc& desc) { m_factoryDesc = desc; }
        void addSegment(const GeometrySegment& segment);
        void segments(const std::vector<GeometrySegment>& segments);
        void position3s(const std::vector<MathLib::Vector3>& positions);
        void position4s(const std::vector<MathLib::Vector4>& positions);
        void normals(const std::vector<MathLib::Vector3>& normals);
        void diffuseColors(const std::vector<MathLib::Vector4>& colors);
        void specularColors(const std::vector<MathLib::Vector4>& colors);
        void addTexture2DCoords(const std::vector<MathLib::Vector2>& coords);
        void addTexture1DCoords(const std::vector<float>& coords);
        void addTexture3DCoords(const std::vector<MathLib::Vector3>& coords);
        void paletteIndices(const std::vector<unsigned>& indices);
        void weights(const std::vector<float>& weights, unsigned weight_count);
        void tangents(const std::vector<MathLib::Vector4>& tangents);
        void indices(const std::vector<unsigned>& indices) { m_indices = indices; }
        void vertexCapacity(unsigned capacity) { m_vtxCapacity = capacity; }
        void indexCapacity(unsigned capacity) { m_idxCapacity = capacity; }
        void vertexUsedCount(unsigned count) { m_vtxUsedCount = count; }
        void indexUsedCount(unsigned count) { m_idxUsedCount = count; }
        virtual void topology(Graphics::PrimitiveTopology topology) { m_topology = static_cast<unsigned>(topology); }
        void geometryBound(const Engine::BoundingVolume& bounding) { m_geometryBound = bounding; }

        void computeAlignedBox();
        [[nodiscard]] const Engine::BoundingVolume& geometryBound() const { return m_geometryBound; }

    protected:
        void serializeNonVertexAttributesToGenericDto(Engine::GenericDto& dto) const;

    protected:
        GeometryId m_id;
        Engine::FactoryDesc m_factoryDesc;
        Graphics::VertexFormatCode m_vertexFormat;
        std::vector<unsigned> m_segments;
        std::optional<std::vector<MathLib::Vector3>> m_position3s;
        std::optional<std::vector<MathLib::Vector4>> m_position4s;
        std::optional<std::vector<MathLib::Vector3>> m_normals;
        std::optional<std::vector<MathLib::Vector4>> m_diffuseColors;
        std::optional<std::vector<MathLib::Vector4>> m_specularColors;
        Engine::GenericDtoCollection m_texCoords;
        std::optional<std::vector<unsigned>> m_paletteIndices;
        std::optional<std::vector<float>> m_weights;
        std::optional<std::vector<MathLib::Vector4>> m_tangents;
        std::optional<std::vector<unsigned>> m_indices;
        unsigned m_vtxCapacity;
        unsigned m_idxCapacity;
        unsigned m_vtxUsedCount;
        unsigned m_idxUsedCount;
        unsigned m_topology;
        Engine::BoundingVolume m_geometryBound;
    };

    class TextureCoordinateDisassembler;

    class GeometryDisassembler
    {
    public:
        GeometryDisassembler();
        virtual ~GeometryDisassembler() = default;
        GeometryDisassembler(const GeometryDisassembler&) = default;
        GeometryDisassembler(GeometryDisassembler&&) = default;
        GeometryDisassembler& operator=(const GeometryDisassembler&) = default;
        GeometryDisassembler& operator=(GeometryDisassembler&&) = default;

        virtual void disassemble(const Engine::GenericDto& dto);

        [[nodiscard]] const GeometryId& id() const { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        [[nodiscard]] const Graphics::VertexFormatCode& vertexFormat() const { return m_vertexFormat; }
        [[nodiscard]] const std::vector<GeometrySegment>& segments() const { return m_segments; };
        [[nodiscard]] const std::optional<std::vector<MathLib::Vector3>>& position3s() const { return m_position3s; }
        [[nodiscard]] const std::optional<std::vector<MathLib::Vector4>>& position4s() const { return m_position4s; }
        [[nodiscard]] const std::optional<std::vector<MathLib::Vector3>>& normals() const { return m_normals; }
        [[nodiscard]] const std::optional<std::vector<MathLib::Vector4>>& diffuseColors() const { return m_diffuseColors; }
        [[nodiscard]] const std::optional<std::vector<MathLib::Vector4>>& specularColors() const { return m_specularColors; }
        [[nodiscard]] const std::optional<std::vector<DimensionalTextureCoordinate>>& textureCoordinates() const { return m_textureCoordinates; }
        [[nodiscard]] const std::optional<std::vector<unsigned>>& paletteIndices() const { return m_paletteIndices; }
        [[nodiscard]] const std::optional<std::vector<float>>& weights() const { return m_weights; }
        [[nodiscard]] const std::optional<std::vector<MathLib::Vector4>>& tangents() const { return m_tangents; }
        [[nodiscard]] const std::optional<std::vector<unsigned>>& indices() const { return m_indices; }
        [[nodiscard]] unsigned vertexCapacity() const { return m_vtxCapacity; }
        [[nodiscard]] unsigned indexCapacity() const { return m_idxCapacity; }
        [[nodiscard]] unsigned vertexUsedCount() const { return m_vtxUsedCount; }
        [[nodiscard]] unsigned indexUsedCount() const { return m_idxUsedCount; }
        [[nodiscard]] Graphics::PrimitiveTopology topology() const { return m_topology; }
        [[nodiscard]] const Engine::BoundingVolume& geometryBound() const { return m_geometryBound; }

    protected:
        void deserializeNonVertexAttributesFromGenericDto(const Engine::GenericDto& dto);

    protected:
        GeometryId m_id;
        Engine::FactoryDesc m_factoryDesc;
        Graphics::VertexFormatCode m_vertexFormat;
        std::vector<GeometrySegment> m_segments;
        std::optional<std::vector<MathLib::Vector3>> m_position3s;
        std::optional<std::vector<MathLib::Vector4>> m_position4s;
        std::optional<std::vector<MathLib::Vector3>> m_normals;
        std::optional<std::vector<MathLib::Vector4>> m_diffuseColors;
        std::optional<std::vector<MathLib::Vector4>> m_specularColors;
        std::optional<std::vector<DimensionalTextureCoordinate>> m_textureCoordinates;
        std::optional<std::vector<unsigned>> m_paletteIndices;
        std::optional<std::vector<float>> m_weights;
        std::optional<std::vector<MathLib::Vector4>> m_tangents;
        std::optional<std::vector<unsigned>> m_indices;
        unsigned m_vtxCapacity;
        unsigned m_idxCapacity;
        unsigned m_vtxUsedCount;
        unsigned m_idxUsedCount;
        Graphics::PrimitiveTopology m_topology;
        Engine::BoundingVolume m_geometryBound;
    };
}

#endif // GEOMETRY_ASSEMBLER_H

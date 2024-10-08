/*********************************************************************
 * \file   TextureCoordinateAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef TEXTURE_COORDINATE_ASSEMBLER_H
#define TEXTURE_COORDINATE_ASSEMBLER_H

#include "GameEngine/GenericDto.h"
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include <vector>
#include <optional>

namespace Enigma::Geometries
{
    class DimensionalTextureCoordinate
    {
    public:
        DimensionalTextureCoordinate() = default;
        DimensionalTextureCoordinate(const std::vector<MathLib::Vector2>& coords) { texture2DCoords(coords); }
        DimensionalTextureCoordinate(const std::vector<float>& coords) { texture1DCoords(coords); }
        DimensionalTextureCoordinate(const std::vector<MathLib::Vector3>& coords) { texture3DCoords(coords); }

        void texture2DCoords(const std::vector<MathLib::Vector2>& coords);
        void texture1DCoords(const std::vector<float>& coords);
        void texture3DCoords(const std::vector<MathLib::Vector3>& coords);

        [[nodiscard]] std::optional<std::vector<MathLib::Vector2>> texture2DCoords() const { return m_2dCoords; }
        [[nodiscard]] std::optional<std::vector<float>> texture1DCoords() const { return m_1dCoords; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> texture3DCoords() const { return m_3dCoords; }

    protected:
        std::optional<std::vector<MathLib::Vector2>> m_2dCoords;
        std::optional<std::vector<float>> m_1dCoords;
        std::optional<std::vector<MathLib::Vector3>> m_3dCoords;
    };

    class TextureCoordinateAssembler
    {
    public:
        TextureCoordinateAssembler() = default;

        Engine::GenericDto assemble() const;

        void textureCoordinate(const DimensionalTextureCoordinate& coords) { m_dimensionalCoords = coords; }
        void texture2DCoords(const std::vector<MathLib::Vector2>& coords) { m_dimensionalCoords.texture2DCoords(coords); }
        void texture1DCoords(const std::vector<float>& coords) { m_dimensionalCoords.texture1DCoords(coords); }
        void texture3DCoords(const std::vector<MathLib::Vector3>& coords) { m_dimensionalCoords.texture3DCoords(coords); }

    protected:
        DimensionalTextureCoordinate m_dimensionalCoords;
    };

    class TextureCoordinateDisassembler
    {
    public:
        TextureCoordinateDisassembler() = default;

        void disassemble(const Engine::GenericDto& dto);

        [[nodiscard]] DimensionalTextureCoordinate textureCoordinate() const { return m_dimensionalCoords; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector2>> texture2DCoords() const { return m_dimensionalCoords.texture2DCoords(); }
        [[nodiscard]] std::optional<std::vector<float>> texture1DCoords() const { return m_dimensionalCoords.texture1DCoords(); }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> texture3DCoords() const { return m_dimensionalCoords.texture3DCoords(); }

    protected:
        DimensionalTextureCoordinate m_dimensionalCoords;
    };
}

#endif // TEXTURE_COORDINATE_ASSEMBLER_H

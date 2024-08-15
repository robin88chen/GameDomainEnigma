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
    class TextureCoordinateAssembler
    {
    public:
        TextureCoordinateAssembler() = default;

        Engine::GenericDto assemble() const;

        void texture2DCoords(const std::vector<MathLib::Vector2>& coords) { m_2dCoords = coords; }
        void texture1DCoords(const std::vector<float>& coords) { m_1dCoords = coords; }
        void texture3DCoords(const std::vector<MathLib::Vector3>& coords) { m_3dCoords = coords; }

    protected:
        std::optional<std::vector<MathLib::Vector2>> m_2dCoords;
        std::optional<std::vector<float>> m_1dCoords;
        std::optional<std::vector<MathLib::Vector3>> m_3dCoords;
    };

    class TextureCoordinateDisassembler
    {
    public:
        TextureCoordinateDisassembler() = default;

        void disassemble(const Engine::GenericDto& dto);

        [[nodiscard]] std::optional<std::vector<MathLib::Vector2>> texture2DCoords() const { return m_2dCoords; }
        [[nodiscard]] std::optional<std::vector<float>> texture1DCoords() const { return m_1dCoords; }
        [[nodiscard]] std::optional<std::vector<MathLib::Vector3>> texture3DCoords() const { return m_3dCoords; }

    protected:
        std::optional<std::vector<MathLib::Vector2>> m_2dCoords;
        std::optional<std::vector<float>> m_1dCoords;
        std::optional<std::vector<MathLib::Vector3>> m_3dCoords;
    };
}

#endif // TEXTURE_COORDINATE_ASSEMBLER_H

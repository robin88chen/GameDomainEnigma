#include "TextureCoordinateAssembler.h"

using namespace Enigma::Geometries;

static std::string TOKEN_2D_COORDS = "2DCoords";
static std::string TOKEN_1D_COORDS = "1DCoords";
static std::string TOKEN_3D_COORDS = "3DCoords";

Enigma::Engine::GenericDto TextureCoordinateAssembler::assemble() const
{
    Engine::GenericDto dto;
    if (m_2dCoords)
    {
        dto.addOrUpdate(TOKEN_2D_COORDS, m_2dCoords.value());
    }
    else if (m_1dCoords)
    {
        dto.addOrUpdate(TOKEN_1D_COORDS, m_1dCoords.value());
    }
    else if (m_3dCoords)
    {
        dto.addOrUpdate(TOKEN_3D_COORDS, m_3dCoords.value());
    }
    return dto;
}

void TextureCoordinateDisassembler::disassemble(const Engine::GenericDto& dto)
{
    if (dto.hasValue(TOKEN_2D_COORDS))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector2>>(TOKEN_2D_COORDS)) m_2dCoords = v.value();
    }
    else if (dto.hasValue(TOKEN_1D_COORDS))
    {
        if (auto v = dto.tryGetValue<std::vector<float>>(TOKEN_1D_COORDS)) m_1dCoords = v.value();
    }
    else if (dto.hasValue(TOKEN_3D_COORDS))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector3>>(TOKEN_3D_COORDS)) m_3dCoords = v.value();
    }
}

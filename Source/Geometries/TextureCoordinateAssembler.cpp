#include "TextureCoordinateAssembler.h"
#include <cassert>

using namespace Enigma::Geometries;

static std::string TOKEN_2D_COORDS = "2DCoords";
static std::string TOKEN_1D_COORDS = "1DCoords";
static std::string TOKEN_3D_COORDS = "3DCoords";

void DimensionalTextureCoordinate::texture2DCoords(const std::vector<MathLib::Vector2>& coords)
{
    assert((!m_1dCoords.has_value()) && (!m_3dCoords.has_value()));
    m_2dCoords = coords;
}

void DimensionalTextureCoordinate::texture1DCoords(const std::vector<float>& coords)
{
    assert((!m_2dCoords.has_value()) && (!m_3dCoords.has_value()));
    m_1dCoords = coords;
}

void DimensionalTextureCoordinate::texture3DCoords(const std::vector<MathLib::Vector3>& coords)
{
    assert((!m_1dCoords.has_value()) && (!m_2dCoords.has_value()));
    m_3dCoords = coords;
}

Enigma::Engine::GenericDto TextureCoordinateAssembler::assemble() const
{
    Engine::GenericDto dto;
    if (m_dimensionalCoords.texture2DCoords())
    {
        dto.addOrUpdate(TOKEN_2D_COORDS, m_dimensionalCoords.texture2DCoords().value());
    }
    else if (m_dimensionalCoords.texture1DCoords())
    {
        dto.addOrUpdate(TOKEN_1D_COORDS, m_dimensionalCoords.texture1DCoords().value());
    }
    else if (m_dimensionalCoords.texture3DCoords())
    {
        dto.addOrUpdate(TOKEN_3D_COORDS, m_dimensionalCoords.texture3DCoords().value());
    }
    return dto;
}

void TextureCoordinateDisassembler::disassemble(const Engine::GenericDto& dto)
{
    if (dto.hasValue(TOKEN_2D_COORDS))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector2>>(TOKEN_2D_COORDS)) m_dimensionalCoords.texture2DCoords(v.value());
    }
    else if (dto.hasValue(TOKEN_1D_COORDS))
    {
        if (auto v = dto.tryGetValue<std::vector<float>>(TOKEN_1D_COORDS)) m_dimensionalCoords.texture1DCoords(v.value());
    }
    else if (dto.hasValue(TOKEN_3D_COORDS))
    {
        if (auto v = dto.tryGetValue<std::vector<MathLib::Vector3>>(TOKEN_3D_COORDS)) m_dimensionalCoords.texture3DCoords(v.value());
    }
}

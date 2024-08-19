#include "VertexDescription.h"
#include "Frameworks/TokenVector.h"
#include "Frameworks/StringFormat.h"
#include <cassert>

using namespace Enigma::Graphics;

//-------------------------- Vertex Format Code --------------------------------------------------
/*VertexFormatCode::VertexFormatCode()
{
    m_fvfCode = 0;
    m_texCount = 0;
    memset(&m_texCoordSize, 0, sizeof(m_texCoordSize));
};

VertexFormatCode::VertexFormatCode(unsigned int code, unsigned int tex_count,
    unsigned char size0, unsigned char size1, unsigned char size2, unsigned char size3,
    unsigned char size4, unsigned char size5, unsigned char size6, unsigned char size7)
{
    m_fvfCode = code; m_texCount = tex_count;
    m_texCoordSize[0] = size0; m_texCoordSize[1] = size1;
    m_texCoordSize[2] = size2; m_texCoordSize[3] = size3;
    m_texCoordSize[4] = size4; m_texCoordSize[5] = size5;
    m_texCoordSize[6] = size6; m_texCoordSize[7] = size7;
};*/



VertexDescription::VertexDescription()
{
    initialize();
}

void VertexDescription::initialize()
{
    m_totalVertexSize = 0;
    m_numElements = 0;
    m_positionOffset = 0;
    m_weightOffset = -1;
    m_paletteIndexOffset = -1;
    m_blendWeightCount = 0;
    m_normalOffset = -1;
    m_colorOffset = -1;
    m_specularOffset = -1;
    m_texCoordOffset[0] = -1;
    m_texCoordOffset[1] = -1;
    m_texCoordOffset[2] = -1;
    m_texCoordOffset[3] = -1;
    m_texCoordOffset[4] = -1;
    m_texCoordOffset[5] = -1;
    m_texCoordOffset[6] = -1;
    m_texCoordOffset[7] = -1;
    m_tangentOffset = -1;
    m_biNormalOffset = -1;
    m_texCoordSize[0] = 0;
    m_texCoordSize[1] = 0;
    m_texCoordSize[2] = 0;
    m_texCoordSize[3] = 0;
    m_texCoordSize[4] = 0;
    m_texCoordSize[5] = 0;
    m_texCoordSize[6] = 0;
    m_texCoordSize[7] = 0;
}

int VertexDescription::weightOffset(unsigned weight_idx) const
{
    if (blendWeightCount() <= static_cast<int>(weight_idx)) return -1;
    return weightOffset() + static_cast<int>(weight_idx);
}

int VertexDescription::diffuseColorOffset(ColorNumeric type) const
{
    if ((type == ColorNumeric::UInt) && (m_colorDimension == 1)) return m_colorOffset;
    if ((type == ColorNumeric::Float) && (m_colorDimension == 4)) return m_colorOffset;
    return -1;
}

int VertexDescription::specularColorOffset(ColorNumeric type) const
{
    if ((type == ColorNumeric::UInt) && (m_specularDimension == 1)) return m_specularOffset;
    if ((type == ColorNumeric::Float) && (m_specularDimension == 4)) return m_specularOffset;
    return -1;
}

int VertexDescription::textureCoordOffset(unsigned stage) const
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return m_texCoordOffset[stage];
}

int VertexDescription::textureCoordSize(unsigned stage) const
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    return m_texCoordSize[stage];
}

int VertexDescription::tangentOffset() const
{
    if ((m_tangentOffset >= 0) && (m_tangentDimension == 4)) return m_tangentOffset;
    return -1;
}

bool VertexDescription::hasTextureCoord(unsigned stage, unsigned dimension) const
{
    assert(stage < VertexFormatCode::MAX_TEX_COORD);
    assert((dimension > 0) && (dimension <= 3));
    return (m_texCoordOffset[stage] >= 0) && (m_texCoordSize[stage] == static_cast<int>(dimension));
}

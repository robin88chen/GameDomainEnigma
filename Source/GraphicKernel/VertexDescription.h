/*********************************************************************
 * \file   VertexDescription.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _VERTEX_DESCRIPTION_H
#define _VERTEX_DESCRIPTION_H

#include <string>
#include "VertexFormatCode.h"

namespace Enigma::Graphics
{
    /** Vertex Format Code Structure */
    /*struct VertexFormatCode
    {


        VertexFormatCode();
        VertexFormatCode(unsigned int code, unsigned int tex_count = 0,
            unsigned char size0 = 0, unsigned char size1 = 0, unsigned char size2 = 0, unsigned char size3 = 0,
            unsigned char size4 = 0, unsigned char size5 = 0, unsigned char size6 = 0, unsigned char size7 = 0);

        bool operator==(const VertexFormatCode& data) const;
        bool operator !=(const VertexFormatCode& data) const;
        bool operator <(const VertexFormatCode& data) const;

    };*/

    class VertexDescription
    {
    public:
        enum class ColorNumeric
        {
            UInt,
            Float
        };
    public:
        VertexDescription();
        ~VertexDescription() = default;
        VertexDescription(const VertexDescription&) = default;
        VertexDescription(VertexDescription&&) = default;
        VertexDescription& operator=(const VertexDescription&) = default;
        VertexDescription& operator=(VertexDescription&&) = default;

        void initialize();

        int positionOffset() const { return m_positionOffset; }
        int weightOffset() const { return m_weightOffset; }
        int weightOffset(unsigned weight_idx) const;
        int paletteIndexOffset() const { return m_paletteIndexOffset; }
        int normalOffset() const { return m_normalOffset; }
        int diffuseColorOffset() const { return m_colorOffset; }
        int diffuseColorOffset(ColorNumeric type) const;
        int specularColorOffset() const { return m_specularOffset; }
        int specularColorOffset(ColorNumeric type) const;
        int textureCoordOffset(unsigned stage) const;
        int textureCoordSize(unsigned stage) const;
        int tangentOffset() const;
        int binormalOffset() const { return m_biNormalOffset; }
        int blendWeightCount() const { return m_blendWeightCount; }
        int totalVertexSize() const { return m_totalVertexSize; }
        int numberOfElements() const { return m_numElements; }

        int positionDimension() const { return m_posVecDimension; }
        int diffuseColorDimension() const { return m_colorDimension; }
        int specularColorDimension() const { return m_specularDimension; }
        int tangentDimension() const { return m_tangentDimension; }

        bool hasPosition3() const { return (positionOffset() >= 0) && (positionDimension() == 3); }
        bool hasPosition4() const { return (positionOffset() >= 0) && (positionDimension() == 4); }
        bool hasNormal() const { return normalOffset() >= 0; }
        bool hasDiffuseColor(ColorNumeric type) const { return diffuseColorOffset(type) >= 0; }
        bool hasSpecularColor(ColorNumeric type) const { return specularColorOffset(type) >= 0; }
        bool hasTextureCoord(unsigned stage, unsigned dimension) const;
        bool hasPaletteIndex() const { return paletteIndexOffset() >= 0; }
        bool hasBlendWeight() const { return (weightOffset() >= 0) && (blendWeightCount() > 0); }
        bool hasTangent() const { return tangentOffset() >= 0; }

    private:
        friend class VertexFormatCode;
        // offset -- 從vertex的基本位置算起
        int m_positionOffset;
        int m_weightOffset;  // blending weight array
        int m_paletteIndexOffset;  // matrix palette index
        int m_normalOffset;
        int m_colorOffset;
        int m_specularOffset;
        int m_texCoordOffset[VertexFormatCode::MAX_TEX_COORD];
        int m_tangentOffset;
        int m_biNormalOffset;

        int m_blendWeightCount;

        int m_totalVertexSize;  // Size of the whole vertices..(in bytes)

        int m_texCoordSize[VertexFormatCode::MAX_TEX_COORD];  // 1D, 2D, 3D ?

        int m_posVecDimension;
        int m_colorDimension;  // 1: dword, 4: 4 float
        int m_specularDimension;  // 1: dword, 4: 4 float
        int m_tangentDimension; // 3 or 4
        int m_numElements;
    };
}

#endif // _VERTEX_DESCRIPTION_H

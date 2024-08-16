/*********************************************************************
 * \file   VertexFormatCode.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef VERTEX_FORMAT_CODE_H
#define VERTEX_FORMAT_CODE_H

#include <string>

namespace Enigma::Graphics
{
    class VertexDescription;

    class VertexFormatCode
    {
    public:
        enum
        {
            MAX_TEX_COORD = 8,
        };
        /** Code Constant */
        enum FormatCode
        {
            XYZ = 0x001,
            XYZRHW = 0x002,
            XYZB1 = 0x003,
            XYZB2 = 0x004,
            XYZB3 = 0x005,
            XYZB4 = 0x006,
            XYZB5 = 0x007,
            XYZW = 0x008,

            POSITION_MASK = 0x00f,

            NORMAL = 0x010,
            PSIZE = 0x020,
            DIFFUSE = 0x040,
            SPECULAR = 0x080,
            TANGENT = 0x100,
            BINORMAL = 0x200,
            FLOAT_DIFFUSE = 0x400,
            FLOAT_SPECULAR = 0x800,
            LASTBETA_UBYTE4 = 0x1000,
            LASTBETA_COLOR = 0x8000,
        };

    public:
        VertexFormatCode();

        bool operator==(const VertexFormatCode& data) const;
        bool operator !=(const VertexFormatCode& data) const;
        bool operator <(const VertexFormatCode& data) const;

        void addPosition3();
        void addPosition4();
        void addNormal();
        void addPointSize();
        void addDiffuse();
        void addSpecular();
        void addTangent();
        void addBinormal();
        void addPaletteIndex();
        void addWeights(unsigned count);
        void addTextureCoord(unsigned stage, unsigned size);
        void addFloatDiffuse();
        void addFloatSpecular();
        void addLastBetaUByte4();
        void replacePositionFormat(FormatCode format);
        FormatCode getPositionFormat() const;

        VertexDescription calculateVertexSize();
        /** Make Code From String
        @remarks
        string format: \n  字串以底線連接token, 每個token表示為: \n
        "xyz", "xyzrhw", "xyzw", "xyzb1", "xyzb2", "xyzb3", "xyzb4", "xyzb5" :位置 \n
        "nor" : normal \n
        "psize" : point size \n
        "diffuse", "specular" : diffuse/specular color (1 byte)\n
        "fdiffuse","fspecular" : diffuse/specular color (4 float) \n
        "tangent", "binormal" : tangent/binormal vector; T/B同時存在: 3 float; 只有tangent : tangent 4 float \n
        "betabyte", "betacolor" : last beta ubyte/last beta color \n
        "texn(x,x,x)" : n表示有多少層貼圖(1~8),後面()裡為每層貼圖的貼圖軸數量
        */
        void fromString(const std::string& fvf_string);
        /** Make String From Code */
        std::string toString() const;
        //todo : format code 常數物件；與實作脫離的格式名稱字串 (ex: "PositionNormalSingleUV")

    private:
        void updateFvfCodeOfPaletteWeight();

    private:
        bool m_hasPosition3;
        bool m_hasPosition4;
        bool m_hasPaletteIndex;
        unsigned m_weightCount;
        unsigned int m_fvfCode;  /**< 一般性的fvf */
        unsigned int m_texCount;  /**< 貼圖軸數量 */
        unsigned char m_texCoordSize[MAX_TEX_COORD];  /**<  每一個stage貼圖軸size (1~4) \n 一定要依照順序，不能跳過某一組 */
    };
}

#endif // VERTEX_FORMAT_CODE_H

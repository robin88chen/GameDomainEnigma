/*********************************************************************
 * \file   IVertexDeclaration.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef VERTEX_DECLARATION_INTERFACE_H
#define VERTEX_DECLARATION_INTERFACE_H

#include <string>
#include <memory>

namespace Enigma::Graphics
{
    class IVertexShader;
    using IVertexShaderPtr = std::shared_ptr<IVertexShader>;

    /** vertex format declaration class */
    class IVertexDeclaration : public std::enable_shared_from_this<IVertexDeclaration>
    {
    public:
        struct VertexDesc;

        /** Vertex Format Code Structure */
        struct VertexFormatCode
        {
            enum
            {
                MAX_TEX_COORD = 8,
            };
            /** Code Constant */
            enum
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
                F_DIFFUSE = 0x400,
                F_SPECULAR = 0x800,
                LASTBETA_UBYTE4 = 0x1000,
                LASTBETA_COLOR = 0x8000,
            };

            unsigned int m_fvfCode;  /**< 一般性的fvf */
            unsigned int m_texCount;  /**< 貼圖軸數量 */
            unsigned char m_texCoordSize[MAX_TEX_COORD];  /**<  每一個stage貼圖軸size (1~4) \n 一定要依照順序，不能跳過某一組 */

            VertexFormatCode();
            VertexFormatCode(unsigned int code, unsigned int tex_count = 0,
                unsigned char size0 = 0, unsigned char size1 = 0, unsigned char size2 = 0, unsigned char size3 = 0,
                unsigned char size4 = 0, unsigned char size5 = 0, unsigned char size6 = 0, unsigned char size7 = 0);

            bool operator==(const VertexFormatCode& data) const;
            bool operator !=(const VertexFormatCode& data) const;
            bool operator <(const VertexFormatCode& data) const;

            int CalculateVertexSize(VertexDesc* desc);
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
            void FromString(const std::string& fvf_string);
            /** Make String From Code */
            std::string ToString() const;
        };

        /** Vertex Desc. */
        struct VertexDesc
        {
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

            VertexDesc();
            void Initialize();
        };

    public:
        IVertexDeclaration(const std::string& name, const std::string& data_vertex_format);
        IVertexDeclaration(const IVertexDeclaration&) = delete;
        IVertexDeclaration(IVertexDeclaration&&) = delete;
        virtual ~IVertexDeclaration();
        IVertexDeclaration& operator=(const IVertexDeclaration&) = delete;
        IVertexDeclaration& operator=(IVertexDeclaration&&) = delete;

        const std::string& GetName() { return m_name; }
        const std::string& GetFormatString() { return m_formatString; };

        const VertexFormatCode& GetVertexFormatCode() const { return m_dataVertexFormatCode; };

        /** is vertex format matched? */
        virtual bool IsMatched(const std::string& data_vertex_format, const IVertexShaderPtr& vtx_shader) = 0;

        unsigned int SizeofDataVertex() const { return (unsigned int)m_dataVertexFormatDesc.m_totalVertexSize; };

    protected:
        std::string m_name;
        std::string m_formatString;
        VertexFormatCode m_dataVertexFormatCode;
        VertexDesc m_dataVertexFormatDesc;
    };
    using IVertexDeclarationPtr = std::shared_ptr<IVertexDeclaration>;
};


#endif // VERTEX_DECLARATION_INTERFACE_H

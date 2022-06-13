/*********************************************************************
 * \file   GraphicAPITypes.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef GRAPHIC_API_TYPES_H
#define GRAPHIC_API_TYPES_H

#ifndef EN_MAKEFOURCC
#define EN_MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
  ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) |       \
  ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

namespace Enigma::Graphics
{
    /**  Surface/Texture FORMAT  */
    struct GraphicFormat
    {
        /** Format Constant, 與D3DFORMAT相同數值 */
        enum
        {
            FMT_UNKNOWN = 0,     /**< UNKNOWN          */

            FMT_R8G8B8 = 20,     /**< R8G8B8           */
            FMT_A8R8G8B8 = 21,     /**< A8R8G8B8         */
            FMT_X8R8G8B8 = 22,     /**< X8R8G8B8         */
            FMT_R5G6B5 = 23,     /**< R5G6B5           */
            FMT_X1R5G5B5 = 24,     /**< X1R5G5B5         */
            FMT_A1R5G5B5 = 25,     /**< A1R5G5B5         */
            FMT_A4R4G4B4 = 26,     /**< A4R4G4B4         */
            FMT_R3G3B2 = 27,     /**< R3G3B2           */
            FMT_A8 = 28,     /**< A8               */
            FMT_A8R3G3B2 = 29,     /**< A8R3G3B2         */
            FMT_X4R4G4B4 = 30,     /**< X4R4G4B4         */
            FMT_A2B10G10R10 = 31,     /**< A2B10G10R10      */
            FMT_A8B8G8R8 = 32,     /**< A8B8G8R8         */
            FMT_X8B8G8R8 = 33,     /**< X8B8G8R8         */
            FMT_G16R16 = 34,     /**< G16R16           */
            FMT_A2R10G10B10 = 35,     /**< A2R10G10B10      */
            FMT_A16B16G16R16 = 36,     /**< A16B16G16R16     */

            FMT_A8P8 = 40,     /**< A8P8             */
            FMT_P8 = 41,     /**< P8               */

            FMT_L8 = 50,     /**< L8               */
            FMT_A8L8 = 51,     /**< A8L8             */
            FMT_A4L4 = 52,     /**< A4L4             */

            FMT_V8U8 = 60,     /**< V8U8             */
            FMT_L6V5U5 = 61,     /**< L6V5U5           */
            FMT_X8L8V8U8 = 62,     /**< X8L8V8U8         */
            FMT_Q8W8V8U8 = 63,     /**< Q8W8V8U8         */
            FMT_V16U16 = 64,     /**< V16U16           */
            FMT_A2W10V10U10 = 67,     /**< A2W10V10U10      */

            FMT_UYVY = EN_MAKEFOURCC('U', 'Y', 'V', 'Y'),     /**< UYVY          */
            FMT_R8G8_B8G8 = EN_MAKEFOURCC('R', 'G', 'B', 'G'),     /**< R8G8_B8G8     */
            FMT_YUY2 = EN_MAKEFOURCC('Y', 'U', 'Y', '2'),     /**< YUY2          */
            FMT_G8R8_G8B8 = EN_MAKEFOURCC('G', 'R', 'G', 'B'),     /**< G8R8_G8B8     */
            FMT_DXT1 = EN_MAKEFOURCC('D', 'X', 'T', '1'),     /**< DXT1          */
            FMT_DXT2 = EN_MAKEFOURCC('D', 'X', 'T', '2'),     /**< DXT2          */
            FMT_DXT3 = EN_MAKEFOURCC('D', 'X', 'T', '3'),     /**< DXT3          */
            FMT_DXT4 = EN_MAKEFOURCC('D', 'X', 'T', '4'),     /**< DXT4          */
            FMT_DXT5 = EN_MAKEFOURCC('D', 'X', 'T', '5'),     /**< DXT5          */

            FMT_D16_LOCKABLE = 70,     /**<  D16_LOCKABLE    */
            FMT_D32 = 71,     /**<  D32             */
            FMT_D15S1 = 73,     /**<  D15S1           */
            FMT_D24S8 = 75,     /**<  D24S8           */
            FMT_D24X8 = 77,     /**<  D24X8           */
            FMT_D24X4S4 = 79,     /**<  D24X4S4         */
            FMT_D16 = 80,     /**<  D16             */

            FMT_D32F_LOCKABLE = 82,     /**<  D32F_LOCKABLE   */
            FMT_D24FS8 = 83,     /**<  D24FS8          */


            FMT_L16 = 81,     /**<  L16             */

            FMT_VERTEXDATA = 100,     /**<  VERTEXDATA      */
            FMT_INDEX16 = 101,     /**<  INDEX16         */
            FMT_INDEX32 = 102,     /**<  INDEX32         */

            FMT_Q16W16V16U16 = 110,     /**<  Q16W16V16U16    */

            FMT_MULTI2_ARGB8 = EN_MAKEFOURCC('M', 'E', 'T', '1'),     /**<  MULTI2_ARGB8  */

            // Floating point surface formats

            // s10e5 formats (16-bits per channel)
            FMT_R16F = 111,     /**<  R16F           */
            FMT_G16R16F = 112,     /**<  G16R16F        */
            FMT_A16B16G16R16F = 113,     /**<  A16B16G16R16F  */

            // IEEE s23e8 formats (32-bits per channel)
            FMT_R32F = 114,     /**<  R32F            */
            FMT_G32R32F = 115,     /**<  G32R32F         */
            FMT_A32B32G32R32F = 116,     /**<  A32B32G32R32F   */

            FMT_CxV8U8 = 117,     /**<  CxV8U8          */


            FMT_FORCE_DWORD = 0x7fffffff
        };
        unsigned int fmt;
        GraphicFormat()
        {
            fmt = FMT_UNKNOWN;
        };
        GraphicFormat(unsigned int ufmt)
        {
            fmt = ufmt;
        };
        unsigned int ColorChannelBits() const;
        unsigned int AlphaChannelBits() const;
        unsigned int PixelBits() const;
        unsigned int DepthBits() const;
        unsigned int StencilBits() const;
        static GraphicFormat ColorFormat(int r, int g, int b, int a);
        static GraphicFormat DepthFormat(int d, int s);
    };

    /** @addtogroup primitive_topology Primitive Topology
    @{ */
    /** Primitive Topology  */
    // 與D3D_PRIMITIVE_TOPOLOGY數值相同
    enum class PrimitiveTopology
    {
        Topology_Undefine = 0,
        Topology_PointList = 1,
        Topology_LineList = 2,
        Topology_LineStrip = 3,
        Topology_TriangleList = 4,
        Topology_TriangleStrip = 5,
    };
    //@}
    /** Feature Levels */
    // 與 D3D_FEATURE_LEVEL 數值相同
    enum GraphicFeatureLevel
    {
        FeatureLevel_11_1 = 0xb100,
        FeatureLevel_11_0 = 0xb000,
        FeatureLevel_10_1 = 0xa100,
        FeatureLevel_10_0 = 0xa000,
        FeatureLevel_9_3 = 0x9300,
        FeatureLevel_9_2 = 0x9200,
        FeatureLevel_9_1 = 0x9100
    };
}


#endif // GRAPHIC_API_TYPES_H

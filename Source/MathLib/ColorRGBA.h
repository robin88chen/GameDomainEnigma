/********************************************************************
 * \file   ColorRGBA.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _EN_COLOR_RGBA_H
#define _EN_COLOR_RGBA_H

/** @file EnColorRGB.h */
namespace Enigma::MathLib
{
    /** Color RGB Class
    @remarks
    The components are intended to be in the interval [0,1].
    For accessing of colors by array index, the map is 0 = red, 1 = green,
    and 2 = blue.
    */
    class ColorRGBA
    {
    public:
        // Construction.  The components are intended to be in the interval [0,1].
        // For accessing of colors by array index, the map is 0 = red, 1 = green,
        // 2 = blue and 3 = alpha
        ColorRGBA(); ///< initial values (0,0,0,0)
        ColorRGBA(float r, float g, float b, float a);
        /// index -- 0: red, 1: green, 2: blue, 3: alpha
        ColorRGBA(float tuple[4]);
        ColorRGBA(const ColorRGBA& c) = default;
        ColorRGBA(ColorRGBA&&) = default;
        /// rgb = 0xaarrggbb
        ColorRGBA(unsigned int rgba);

        /** @name member access */
        //@{
        operator const float*() const;
        operator float*();
        float operator[](int i) const;
        float& operator[](int i);
        float R() const;
        float& R();
        float G() const;
        float& G();
        float B() const;
        float& B();
        float A() const;
        float& A();
        unsigned int GetRGBA() const;
        //@}

        /// assignment
        ColorRGBA& operator=(const ColorRGBA& c) = default;

        /** @name comparison */
        //@{
        bool operator==(const ColorRGBA& c) const;
        bool operator!=(const ColorRGBA& c) const;
        bool operator<(const ColorRGBA& c) const;
        bool operator<=(const ColorRGBA& c) const;
        bool operator>(const ColorRGBA& c) const;
        bool operator>=(const ColorRGBA& c) const;
        //@}

        /** @name arithmetic operations */
        //@{
        ColorRGBA operator+(const ColorRGBA& c) const;
        ColorRGBA operator-(const ColorRGBA& c) const;
        ColorRGBA operator*(const ColorRGBA& c) const;
        ColorRGBA operator*(float fScalar) const;
        //@}

        /** @name arithmetic updates */
        //@{
        ColorRGBA& operator+=(const ColorRGBA& c);
        ColorRGBA& operator-=(const ColorRGBA& c);
        ColorRGBA& operator*=(const ColorRGBA& c);
        ColorRGBA& operator*=(float scalar);
        //@}

        // Transform the color channels to [0,1].  Clamp sets negative values to
        // zero and values larger than one to one.  ScaleByMax assumes the color
        // channels are nonnegative, finds the maximum color channel, and divides
        // all channels by that value.
        /** Transform the color channels to [0,1].  
        Clamp sets negative values to zero and values larger than one to one. */
        ColorRGBA Clamp() const;
        /** Transform the color channels to [0,1].
        ScaleByMax assumes the color channels are nonnegative, finds the maximum color channel, and divides
        all channels by that value. */
        ColorRGBA ScaleByMax() const;

        static const ColorRGBA BLACK; ///< = (0,0,0,1) 
        static const ColorRGBA WHITE; ///< = (1,1,1,1)
        static const ColorRGBA INVALID; ///< = (-1,-1,-1,-1)
        static const ColorRGBA ZERO; ///< = (0,0,0,0)

    private:
        union
        {
            struct
            {
                float m_r;
                float m_g;
                float m_b;
                float m_a;
            };

            float m_tuple[4];
        };

        unsigned int m_rgbaValue;
    };

    /** scalar * color */
    ColorRGBA operator*(float scalar, const ColorRGBA& c);
};

#endif

/********************************************************************
 * \file   ColorRGB.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   April 2022
 *********************************************************************/
#ifndef _EN_COLOR_RGB_H
#define _EN_COLOR_RGB_H

/** @file ColorRGB.h */
namespace Enigma::MathLib
{
    /** Color RGB Class
    @remarks
    The components are intended to be in the interval [0,1].
     For accessing of colors by array index, the map is 0 = red, 1 = green,
     and 2 = blue.
    */
    class ColorRGB
    {
    public:
        // Construction.  The components are intended to be in the interval [0,1].
        // For accessing of colors by array index, the map is 0 = red, 1 = green,
        // and 2 = blue.
        ColorRGB(); ///< initial values (0,0,0)
        ColorRGB(float r, float g, float b);
        /// index -- 0: red, 1: green, 2: blue
        ColorRGB(float tuple[3]);
        ColorRGB(const ColorRGB& c) = default;
        ColorRGB(ColorRGB&&) = default;
        /// rgb = 0x00rrggbb
        ColorRGB(unsigned int rgb);

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
        unsigned int GetRGB() const;
        //@}

        /// assignment
        ColorRGB& operator=(const ColorRGB&) = default;
        ColorRGB& operator=(ColorRGB&&) = default;

        /** @name comparison */
        //@{
        bool operator==(const ColorRGB& c) const;
        bool operator!=(const ColorRGB& c) const;
        bool operator<(const ColorRGB& c) const;
        bool operator<=(const ColorRGB& c) const;
        bool operator>(const ColorRGB& c) const;
        bool operator>=(const ColorRGB& c) const;
        //@}

        /** @name arithmetic operations */
        //@{
        ColorRGB operator+(const ColorRGB& c) const;
        ColorRGB operator-(const ColorRGB& c) const;
        ColorRGB operator*(const ColorRGB& c) const;
        ColorRGB operator*(float scalar) const;
        //@}

        /** @name arithmetic updates */
        //@{
        ColorRGB& operator+=(const ColorRGB& c);
        ColorRGB& operator-=(const ColorRGB& c);
        ColorRGB& operator*=(const ColorRGB& c);
        ColorRGB& operator*=(float scalar);
        //@}

        // Transform the color channels to [0,1].  Clamp sets negative values to
        // zero and values larger than one to one.  ScaleByMax assumes the color
        // channels are nonnegative, finds the maximum color channel, and divides
        // all channels by that value.
        /** Transform the color channels to [0,1].  
        Clamp sets negative values to zero and values larger than one to one. */
        ColorRGB Clamp() const;
        /** Transform the color channels to [0,1].
        ScaleByMax assumes the color channels are nonnegative, finds the maximum color channel, and divides
         all channels by that value. */
        ColorRGB ScaleByMax() const;

        static const ColorRGB BLACK; ///< = (0,0,0) 
        static const ColorRGB WHITE; ///< = (1,1,1)
        static const ColorRGB INVALID; ///< = (-1,-1,-1)
        static const ColorRGB ZERO; ///< = (0,0,0)

    private:
        float m_tuple[3];
        unsigned int m_rgbValue;
    };

    /** scalar * color */
    ColorRGB operator*(float scalar, const ColorRGB& c);
};

#endif

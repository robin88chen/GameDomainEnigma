#include "ColorRGB.h"
#include <cassert>

using namespace Enigma::MathLib;

const ColorRGB ColorRGB::BLACK(0.0f, 0.0f, 0.0f);
const ColorRGB ColorRGB::WHITE(1.0f, 1.0f, 1.0f);
const ColorRGB ColorRGB::INVALID(-1.0f, -1.0f, -1.0f);
const ColorRGB ColorRGB::ZERO(0.0f, 0.0f, 0.0f);

static unsigned int rgb_float_to_int(float r, float g, float b)
{
    unsigned int rgb = (unsigned char)(r * 255);
    rgb = (rgb << 8) + (unsigned char)(g * 255);
    rgb = (rgb << 8) + (unsigned char)(b * 255);
    return rgb;
}

static void rgb_int_to_float(unsigned int rgb, float& r, float& g, float& b)
{
    unsigned char c = rgb & 0xff;
    b = (float)c / 255.0f;
    c = (rgb >> 8) & 0xff;
    g = (float)c / 255.0f;
    c = (rgb >> 16) & 0xff;
    r = (float)c / 255.0f;
}

ColorRGB::ColorRGB()
{
    m_tuple[0] = 0.0f;
    m_tuple[1] = 0.0f;
    m_tuple[2] = 0.0f;
    m_rgbValue = 0;
}

ColorRGB::ColorRGB(float r, float g, float b)
{
    m_tuple[0] = r;
    m_tuple[1] = g;
    m_tuple[2] = b;
    m_rgbValue = rgb_float_to_int(r, g, b);
}

ColorRGB::ColorRGB(float tuple[3])
{
    m_tuple[0] = tuple[0];
    m_tuple[1] = tuple[1];
    m_tuple[2] = tuple[2];
    m_rgbValue = rgb_float_to_int(tuple[0], tuple[1], tuple[2]);
}

ColorRGB::ColorRGB(unsigned int rgb)
{
    m_rgbValue = rgb;
    rgb_int_to_float(rgb, m_tuple[0], m_tuple[1], m_tuple[2]);
}

ColorRGB::operator const float*() const
{
    return m_tuple;
}

ColorRGB::operator float*()
{
    return m_tuple;
}

float ColorRGB::operator[](int i) const
{
    assert((i < 3) && (i >= 0));
    return m_tuple[i];
}

float& ColorRGB::operator[](int i)
{
    assert((i < 3) && (i >= 0));
    return m_tuple[i];
}

float ColorRGB::R() const
{
    return m_tuple[0];
}

float& ColorRGB::R()
{
    return m_tuple[0];
}

float ColorRGB::G() const
{
    return m_tuple[1];
}

float& ColorRGB::G()
{
    return m_tuple[1];
}

float ColorRGB::B() const
{
    return m_tuple[2];
}

float& ColorRGB::B()
{
    return m_tuple[2];
}

unsigned int ColorRGB::GetRGB() const
{
    return m_rgbValue;
}

bool ColorRGB::operator==(const ColorRGB& c) const
{
    return (m_rgbValue == c.m_rgbValue);
}

bool ColorRGB::operator!=(const ColorRGB& c) const
{
    return (m_rgbValue != c.m_rgbValue);
}

bool ColorRGB::operator<(const ColorRGB& c) const
{
    return (m_rgbValue < c.m_rgbValue);
}

bool ColorRGB::operator<=(const ColorRGB& c) const
{
    return (m_rgbValue <= c.m_rgbValue);
}

bool ColorRGB::operator>(const ColorRGB& c) const
{
    return (m_rgbValue > c.m_rgbValue);
}

bool ColorRGB::operator>=(const ColorRGB& c) const
{
    return (m_rgbValue >= c.m_rgbValue);
}

ColorRGB ColorRGB::operator+(const ColorRGB& c) const
{
    return ColorRGB(
        m_tuple[0] + c.m_tuple[0],
        m_tuple[1] + c.m_tuple[1],
        m_tuple[2] + c.m_tuple[2]);
}

ColorRGB ColorRGB::operator-(const ColorRGB& c) const
{
    return ColorRGB(
        m_tuple[0] - c.m_tuple[0],
        m_tuple[1] - c.m_tuple[1],
        m_tuple[2] - c.m_tuple[2]);
}

ColorRGB ColorRGB::operator*(const ColorRGB& c) const
{
    return ColorRGB(
        m_tuple[0] * c.m_tuple[0],
        m_tuple[1] * c.m_tuple[1],
        m_tuple[2] * c.m_tuple[2]);
}

ColorRGB ColorRGB::operator*(float scalar) const
{
    return ColorRGB(
        scalar * m_tuple[0],
        scalar * m_tuple[1],
        scalar * m_tuple[2]);
}

ColorRGB& ColorRGB::operator+=(const ColorRGB& c)
{
    m_tuple[0] += c.m_tuple[0];
    m_tuple[1] += c.m_tuple[1];
    m_tuple[2] += c.m_tuple[2];
    m_rgbValue = rgb_float_to_int(m_tuple[0], m_tuple[1], m_tuple[2]);
    return *this;
}

ColorRGB& ColorRGB::operator-=(const ColorRGB& c)
{
    m_tuple[0] -= c.m_tuple[0];
    m_tuple[1] -= c.m_tuple[1];
    m_tuple[2] -= c.m_tuple[2];
    m_rgbValue = rgb_float_to_int(m_tuple[0], m_tuple[1], m_tuple[2]);
    return *this;
}

ColorRGB& ColorRGB::operator*=(const ColorRGB& c)
{
    m_tuple[0] *= c.m_tuple[0];
    m_tuple[1] *= c.m_tuple[1];
    m_tuple[2] *= c.m_tuple[2];
    m_rgbValue = rgb_float_to_int(m_tuple[0], m_tuple[1], m_tuple[2]);
    return *this;
}

ColorRGB& ColorRGB::operator*=(float scalar)
{
    m_tuple[0] *= scalar;
    m_tuple[1] *= scalar;
    m_tuple[2] *= scalar;
    m_rgbValue = rgb_float_to_int(m_tuple[0], m_tuple[1], m_tuple[2]);
    return *this;
}

ColorRGB ColorRGB::Clamp() const
{
    ColorRGB c(m_rgbValue);
    if (c.m_tuple[0] > 1.0f) c.m_tuple[0] = 1.0f;
    else if (c.m_tuple[0] < 0.0f) c.m_tuple[0] = 0.0f;
    if (c.m_tuple[1] > 1.0f) c.m_tuple[1] = 1.0f;
    else if (c.m_tuple[1] < 0.0f) c.m_tuple[1] = 0.0f;
    if (c.m_tuple[2] > 1.0f) c.m_tuple[2] = 1.0f;
    else if (c.m_tuple[2] < 0.0f) c.m_tuple[2] = 0.0f;
    c.m_rgbValue = rgb_float_to_int(c.m_tuple[0], c.m_tuple[1], c.m_tuple[2]);
    return c;
}

ColorRGB ColorRGB::ScaleByMax() const
{
    float maxCo = m_tuple[0];
    if (m_tuple[1] > maxCo)
        maxCo = m_tuple[1];
    if (m_tuple[2] > maxCo)
        maxCo = m_tuple[2];

    ColorRGB c(m_rgbValue);
    if (maxCo > 1.0f)
    {
        float invMax = 1.0f / maxCo;
        c.m_tuple[0] *= invMax;
        c.m_tuple[1] *= invMax;
        c.m_tuple[2] *= invMax;
    }
    c.m_rgbValue = rgb_float_to_int(c.m_tuple[0], c.m_tuple[1], c.m_tuple[2]);
    return c;
}

namespace Enigma::MathLib
{
    ColorRGB operator*(float scalar, const ColorRGB& c)
    {
        return ColorRGB(c * scalar);
    }
}
#include "ColorRGBA.h"
#include <cassert>

using namespace Enigma::MathLib;

const ColorRGBA ColorRGBA::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const ColorRGBA ColorRGBA::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
const ColorRGBA ColorRGBA::INVALID(-1.0f, -1.0f, -1.0f, 1.0f);
const ColorRGBA ColorRGBA::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

static unsigned int rgba_float_to_int(float r, float g, float b, float a)
{
    unsigned int rgba = (unsigned char)(a * 255);
    rgba = (rgba << 8) + (unsigned char)(r * 255);
    rgba = (rgba << 8) + (unsigned char)(g * 255);
    rgba = (rgba << 8) + (unsigned char)(b * 255);
    return rgba;
}

static void rgba_int_to_float(unsigned int rgba, float& r, float& g, float& b, float& a)
{
    unsigned char c = rgba & 0xff;
    b = (float)c / 255.0f;
    c = (rgba >> 8) & 0xff;
    g = (float)c / 255.0f;
    c = (rgba >> 16) & 0xff;
    r = (float)c / 255.0f;
    c = (rgba >> 24) & 0xff;
    a = (float)c / 255.0f;
}

ColorRGBA::ColorRGBA()
{
    m_tuple[0] = 0.0f;
    m_tuple[1] = 0.0f;
    m_tuple[2] = 0.0f;
    m_tuple[3] = 0.0f;
    m_rgbaValue = 0;
}

ColorRGBA::ColorRGBA(float r, float g, float b, float a)
{
    m_tuple[0] = r;
    m_tuple[1] = g;
    m_tuple[2] = b;
    m_tuple[3] = a;
    m_rgbaValue = rgba_float_to_int(r, g, b, a);
}

ColorRGBA::ColorRGBA(float tuple[4])
{
    m_tuple[0] = tuple[0];
    m_tuple[1] = tuple[1];
    m_tuple[2] = tuple[2];
    m_tuple[3] = tuple[3];
    m_rgbaValue = rgba_float_to_int(tuple[0], tuple[1], tuple[2], tuple[3]);
}

ColorRGBA::ColorRGBA(unsigned int rgba)
{
    m_rgbaValue = rgba;
    rgba_int_to_float(rgba, m_tuple[0], m_tuple[1], m_tuple[2], m_tuple[3]);
}

ColorRGBA::operator const float*() const
{
    return m_tuple;
}

ColorRGBA::operator float*()
{
    return m_tuple;
}

float ColorRGBA::operator[](int i) const
{
    assert((i < 4) && (i >= 0));
    return m_tuple[i];
}

float& ColorRGBA::operator[](int i)
{
    assert((i < 4) && (i >= 0));
    return m_tuple[i];
}

float ColorRGBA::R() const
{
    return m_tuple[0];
}

float& ColorRGBA::R()
{
    return m_tuple[0];
}

float ColorRGBA::G() const
{
    return m_tuple[1];
}

float& ColorRGBA::G()
{
    return m_tuple[1];
}

float ColorRGBA::B() const
{
    return m_tuple[2];
}

float& ColorRGBA::B()
{
    return m_tuple[2];
}

float ColorRGBA::A() const
{
    return m_tuple[3];
}

float& ColorRGBA::A()
{
    return m_tuple[3];
}

unsigned int ColorRGBA::GetRGBA() const
{
    return m_rgbaValue;
}

bool ColorRGBA::operator==(const ColorRGBA& c) const
{
    return (m_rgbaValue == c.m_rgbaValue);
}

bool ColorRGBA::operator!=(const ColorRGBA& c) const
{
    return (m_rgbaValue != c.m_rgbaValue);
}

bool ColorRGBA::operator<(const ColorRGBA& c) const
{
    return (m_rgbaValue < c.m_rgbaValue);
}

bool ColorRGBA::operator<=(const ColorRGBA& c) const
{
    return (m_rgbaValue <= c.m_rgbaValue);
}

bool ColorRGBA::operator>(const ColorRGBA& c) const
{
    return (m_rgbaValue > c.m_rgbaValue);
}

bool ColorRGBA::operator>=(const ColorRGBA& c) const
{
    return (m_rgbaValue >= c.m_rgbaValue);
}

ColorRGBA ColorRGBA::operator+(const ColorRGBA& c) const
{
    return ColorRGBA(
        m_tuple[0] + c.m_tuple[0],
        m_tuple[1] + c.m_tuple[1],
        m_tuple[2] + c.m_tuple[2],
        m_tuple[3] + c.m_tuple[3]);
}

ColorRGBA ColorRGBA::operator-(const ColorRGBA& c) const
{
    return ColorRGBA(
        m_tuple[0] - c.m_tuple[0],
        m_tuple[1] - c.m_tuple[1],
        m_tuple[2] - c.m_tuple[2],
        m_tuple[3] - c.m_tuple[3]);
}

ColorRGBA ColorRGBA::operator*(const ColorRGBA& c) const
{
    return ColorRGBA(
        m_tuple[0] * c.m_tuple[0],
        m_tuple[1] * c.m_tuple[1],
        m_tuple[2] * c.m_tuple[2],
        m_tuple[3] * c.m_tuple[3]);
}

ColorRGBA ColorRGBA::operator*(float scalar) const
{
    return ColorRGBA(
        scalar * m_tuple[0],
        scalar * m_tuple[1],
        scalar * m_tuple[2],
        scalar * m_tuple[3]);
}

ColorRGBA& ColorRGBA::operator+=(const ColorRGBA& c)
{
    m_tuple[0] += c.m_tuple[0];
    m_tuple[1] += c.m_tuple[1];
    m_tuple[2] += c.m_tuple[2];
    m_tuple[3] += c.m_tuple[3];
    m_rgbaValue = rgba_float_to_int(m_tuple[0], m_tuple[1], m_tuple[2], m_tuple[3]);
    return *this;
}

ColorRGBA& ColorRGBA::operator-=(const ColorRGBA& c)
{
    m_tuple[0] -= c.m_tuple[0];
    m_tuple[1] -= c.m_tuple[1];
    m_tuple[2] -= c.m_tuple[2];
    m_tuple[3] -= c.m_tuple[3];
    m_rgbaValue = rgba_float_to_int(m_tuple[0], m_tuple[1], m_tuple[2], m_tuple[3]);
    return *this;
}

ColorRGBA& ColorRGBA::operator*=(const ColorRGBA& c)
{
    m_tuple[0] *= c.m_tuple[0];
    m_tuple[1] *= c.m_tuple[1];
    m_tuple[2] *= c.m_tuple[2];
    m_tuple[3] *= c.m_tuple[3];
    m_rgbaValue = rgba_float_to_int(m_tuple[0], m_tuple[1], m_tuple[2], m_tuple[3]);
    return *this;
}

ColorRGBA& ColorRGBA::operator*=(float scalar)
{
    m_tuple[0] *= scalar;
    m_tuple[1] *= scalar;
    m_tuple[2] *= scalar;
    m_tuple[3] *= scalar;
    m_rgbaValue = rgba_float_to_int(m_tuple[0], m_tuple[1], m_tuple[2], m_tuple[3]);
    return *this;
}

ColorRGBA ColorRGBA::Clamp() const
{
    ColorRGBA c(m_rgbaValue);
    if (c.m_tuple[0] > 1.0f) c.m_tuple[0] = 1.0f;
    else if (c.m_tuple[0] < 0.0f) c.m_tuple[0] = 0.0f;
    if (c.m_tuple[1] > 1.0f) c.m_tuple[1] = 1.0f;
    else if (c.m_tuple[1] < 0.0f) c.m_tuple[1] = 0.0f;
    if (c.m_tuple[2] > 1.0f) c.m_tuple[2] = 1.0f;
    else if (c.m_tuple[2] < 0.0f) c.m_tuple[2] = 0.0f;
    if (c.m_tuple[3] > 1.0f) c.m_tuple[3] = 1.0f;
    else if (c.m_tuple[3] < 0.0f) c.m_tuple[3] = 0.0f;
    c.m_rgbaValue = rgba_float_to_int(c.m_tuple[0], c.m_tuple[1], c.m_tuple[2], c.m_tuple[3]);
    return c;
}

ColorRGBA ColorRGBA::ScaleByMax() const
{
    float maxCo = m_tuple[0];
    if (m_tuple[1] > maxCo)
        maxCo = m_tuple[1];
    if (m_tuple[2] > maxCo)
        maxCo = m_tuple[2];
    if (m_tuple[3] > maxCo)
        maxCo = m_tuple[3];

    ColorRGBA c(m_rgbaValue);
    if (maxCo > 1.0f)
    {
        float invMax = 1.0f / maxCo;
        c.m_tuple[0] *= invMax;
        c.m_tuple[1] *= invMax;
        c.m_tuple[2] *= invMax;
        c.m_tuple[3] *= invMax;
    }
    c.m_rgbaValue = rgba_float_to_int(c.m_tuple[0], c.m_tuple[1], c.m_tuple[2], c.m_tuple[3]);
    return c;
}

namespace Enigma::MathLib
{
    ColorRGBA operator*(float scalar, const ColorRGBA& c)
    {
        return ColorRGBA(c * scalar);
    }
}
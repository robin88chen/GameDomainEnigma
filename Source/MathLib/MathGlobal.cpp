#include "MathGlobal.h"
#include <cmath>
#include <cfloat>
#include <limits>

using namespace Enigma::MathLib;

float Math::m_epsilonUlp = 1.0f;
const float Math::ZERO_TOLERANCE = std::numeric_limits<float>::epsilon();
const float Math::MAX_FLOAT = FLT_MAX;
const float Math::PI = 4.0f * atanf(1.0f);
const float Math::TWO_PI = 2.0f * Math::PI;
const float Math::HALF_PI = 0.5f * Math::PI;
const float Math::INV_PI = 1.0f / Math::PI;
const float Math::INV_TWO_PI = 1.0f / Math::TWO_PI;
const float Math::DEG_TO_RAD = Math::PI / 180.0f;
const float Math::RAD_TO_DEG = 180.0f / Math::PI;

bool Math::IsEqual(float l, float r)
{
    float diff = fabs(l - r);
    if (diff <= ZERO_TOLERANCE * m_epsilonUlp) return true;
    // code from
    // https://en.cppreference.com/w/cpp/types/numeric_limits/epsilon

    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return (diff <= ZERO_TOLERANCE * std::fabs(l + r) * m_epsilonUlp)
        // unless the result is subnormal
        || (diff < std::numeric_limits<float>::min());
}

Vector3 Math::MaxVectorComponent(const Vector3& vec1, const Vector3& vec2)
{
    Vector3 vecRet = vec1;
    if (vec2.x() > vecRet.x()) vecRet.x() = vec2.x();
    if (vec2.y() > vecRet.y()) vecRet.y() = vec2.y();
    if (vec2.z() > vecRet.z()) vecRet.z() = vec2.z();
    return vecRet;
}

Vector3 Math::MinVectorComponent(const Vector3& vec1, const Vector3& vec2)
{
    Vector3 vecRet = vec1;
    if (vec2.x() < vecRet.x()) vecRet.x() = vec2.x();
    if (vec2.y() < vecRet.y()) vecRet.y() = vec2.y();
    if (vec2.z() < vecRet.z()) vecRet.z() = vec2.z();
    return vecRet;
}

float Math::Epsilon()
{
    return ZERO_TOLERANCE * m_epsilonUlp;
}

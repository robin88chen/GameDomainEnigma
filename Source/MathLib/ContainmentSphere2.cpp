#include "ContainmentSphere2.h"
#include "MathGlobal.h"
#include <cmath>

using namespace Enigma::MathLib;

Sphere2 ContainmentSphere2::MergeSpheres(const Sphere2& sphere0, const Sphere2& sphere1)
{
    Vector2 centerDiff = sphere1.Center() - sphere0.Center();
    float sqrLength = centerDiff.SquaredLength(); // 圓心的距離
    float radiusDiff = sphere1.Radius() - sphere0.Radius();
    float radiusDiffSqr = radiusDiff * radiusDiff; // 半徑的差距

    if (radiusDiffSqr >= sqrLength) // 圓心的距離比半徑差距小，表示完全被包含
    {
        return (radiusDiff >= 0.0f ? sphere1 : sphere0);
    }

    float length = std::sqrt(sqrLength);
    Sphere2 sphere;

    if (length > Math::ZERO_TOLERANCE)
    {
        float coeff = (length + radiusDiff) / (2.0f * length);
        sphere.Center() = sphere0.Center() + coeff * centerDiff;
    }
    else
    {
        sphere.Center() = sphere0.Center();
    }

    sphere.Radius() = 0.5f * (length + sphere0.Radius() + sphere1.Radius());

    return sphere;
}

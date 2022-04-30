#include "ContainmentSphere3.h"
#include "MathGlobal.h"
#include <cassert>
#include <cmath>

using namespace Enigma::MathLib;

Sphere3 ContainmentSphere3::MergeSpheres(const Sphere3& sphere0, const Sphere3& sphere1)
{
    Vector3 centerDiff = sphere1.Center() - sphere0.Center();
    float sqrLength = centerDiff.SquaredLength();
    float radiusDiff = sphere1.Radius() - sphere0.Radius();
    float radiusDiffSqr = radiusDiff * radiusDiff;

    if (radiusDiffSqr >= sqrLength)
    {
        return (radiusDiff >= 0.0f ? sphere1 : sphere0);
    }

    float length = std::sqrt(sqrLength);
    Sphere3 sphere;

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

Sphere3 ContainmentSphere3::ComputeAverageSphere(const Vector3* pos, unsigned int quantity)
{
    assert(pos != 0);
    assert(quantity != 0);
    Vector3 center = pos[0];
    for (unsigned int i = 1; i < quantity; i++)
    {
        center += pos[i];
    }
    center /= (float)(quantity);
    float sq_radius = 0.0f;
    for (unsigned int i = 0; i < quantity; i++)
    {
        Vector3 diff = pos[i] - center;
        float diff_length = diff.SquaredLength();
        if (diff_length > sq_radius) sq_radius = diff_length;
    }
    return Sphere3(center, std::sqrt(sq_radius));
}

Sphere3 ContainmentSphere3::ComputeAverageSphere(const Vector4* pos, unsigned int quantity)
{
    assert(pos != 0);
    assert(quantity != 0);
    Vector3 center = Vector3(pos[0].X(), pos[0].Y(), pos[0].Z());
    for (unsigned int i = 1; i < quantity; i++)
    {
        center += Vector3(pos[i].X(), pos[i].Y(), pos[i].Z());
    }
    center /= (float)(quantity);
    float sq_radius = 0.0f;
    for (unsigned int i = 0; i < quantity; i++)
    {
        Vector3 diff = Vector3(pos[i].X(), pos[i].Y(), pos[i].Z()) - center;
        float diff_length = diff.SquaredLength();
        if (diff_length > sq_radius) sq_radius = diff_length;
    }
    return Sphere3(center, std::sqrt(sq_radius));
}

Sphere3 ContainmentSphere3::ComputeAverageSphere(const float* vert, unsigned int pitch, unsigned int quantity)
{
    assert(vert != 0);
    assert(quantity != 0);
    Vector3 center = Vector3(vert[0], vert[1], vert[2]);
    unsigned int index = pitch;
    for (unsigned int i = 1; i < quantity; i++)
    {
        center += Vector3(vert[index], vert[index + 1], vert[index + 2]);
        index += pitch;
    }
    center /= (float)(quantity);
    float sq_radius = 0.0f;
    index = 0;
    for (unsigned int i = 0; i < quantity; i++)
    {
        Vector3 diff = Vector3(vert[index], vert[index + 1], vert[index + 2]) - center;
        float diff_length = diff.SquaredLength();
        if (diff_length > sq_radius) sq_radius = diff_length;
    }
    return Sphere3(center, std::sqrt(sq_radius));
}

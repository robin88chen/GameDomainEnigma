#include "ContainmentBox2.h"
#include <cassert>
#include <cmath>

using namespace Enigma::MathLib;

Box2 ContainmentBox2::MergeBoxes(const Box2& box0, const Box2& box1)
{
    // Construct a box that contains the input boxes.
    Box2 box;

    // The first guess at the box center.  This value will be updated later
    // after the input box vertices are projected onto axes determined by an
    // average of box axes.
    box.Center() = 0.5f * (box0.Center() + box1.Center());

    // The merged box axes are the averages of the input box axes.  The
    // axes of the second box are negated, if necessary, so they form acute
    // angles with the axes of the first box.
    if (box0.Axis(0).Dot(box1.Axis(0)) >= 0.0f)
    {
        box.Axis(0) = 0.5f * (box0.Axis(0) + box1.Axis(0));
        box.Axis(0) = box.Axis(0).Normalize();
    }
    else
    {
        box.Axis(0) = 0.5f * (box0.Axis(0) - box1.Axis(0));
        box.Axis(0) = box.Axis(0).Normalize();
    }
    box.Axis(1) = -box.Axis(0).Perp();

    // Project the input box vertices onto the merged-box axes.  Each axis
    // D[i] containing the current center C has a minimum projected value
    // min[i] and a maximum projected value max[i].  The corresponding end
    // points on the axes are C+min[i]*D[i] and C+max[i]*D[i].  The point C
    // is not necessarily the midpoint for any of the intervals.  The actual
    // box center will be adjusted from C to a point C' that is the midpoint
    // of each interval,
    //   C' = C + sum_{i=0}^1 0.5*(min[i]+max[i])*D[i]
    // The box extents are
    //   e[i] = 0.5*(max[i]-min[i])

    int i, j;
    float dot;
    Vector2 diff;
    Vector2 pmin = Vector2::ZERO;
    Vector2 pmax = Vector2::ZERO;

    std::array<Vector2, 4> vertex = box0.ComputeVertices();
    for (i = 0; i < 4; ++i)
    {
        diff = vertex[i] - box.Center();
        for (j = 0; j < 2; ++j)
        {
            dot = diff.Dot(box.Axis(j));
            if (dot > pmax[j])
            {
                pmax[j] = dot;
            }
            else if (dot < pmin[j])
            {
                pmin[j] = dot;
            }
        }
    }

    vertex = box1.ComputeVertices();
    for (i = 0; i < 4; ++i)
    {
        diff = vertex[i] - box.Center();
        for (j = 0; j < 2; ++j)
        {
            dot = diff.Dot(box.Axis(j));
            if (dot > pmax[j])
            {
                pmax[j] = dot;
            }
            else if (dot < pmin[j])
            {
                pmin[j] = dot;
            }
        }
    }

    // [min,max] is the axis-aligned box in the coordinate system of the
    // merged box axes.  Update the current box center to be the center of
    // the new box.  Compute the extents based on the new center.
    for (j = 0; j < 2; ++j)
    {
        box.Center() += box.Axis(j) * (0.5f * (pmax[j] + pmin[j]));
        box.Extent(j) = 0.5f * (pmax[j] - pmin[j]);
    }

    return box;
}

bool ContainmentBox2::TestBox2EnvelopBox2(const Box2& box0, const Box2& box1)
{
    std::array<Vector2, 4> vertex = box1.ComputeVertices();
    for (unsigned int i = 0; i < 4; i++)
    {
        Vector2 diff = vertex[i] - box0.Center();
        if (std::fabs(diff.Dot(box0.Axis(0))) > box0.Extent(0)) return false;
        if (std::fabs(diff.Dot(box0.Axis(1))) > box0.Extent(1)) return false;
    }
    return true;
}

bool ContainmentBox2::TestBox2EnvelopSphere2(const Box2& box0, const Sphere2& sphere1)
{
    Vector2 diff = sphere1.Center() - box0.Center();
    if (std::fabs(diff.Dot(box0.Axis(0))) + sphere1.Radius() > box0.Extent(0)) return false;
    if (std::fabs(diff.Dot(box0.Axis(1))) + sphere1.Radius() > box0.Extent(1)) return false;
    return true;
}

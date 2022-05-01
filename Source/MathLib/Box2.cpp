#include "Box2.h"
#include "MathGlobal.h"

using namespace Enigma::MathLib;

Box2::Box2()
{
    m_axis[0] = Vector2::UNIT_X;
    m_axis[1] = Vector2::UNIT_Y;
    m_center = Vector2::ZERO;
    m_extent[0] = m_extent[1] = (float)0.0;
}

Box2::Box2(const Vector2& center, const Vector2 axis[2], const float extent[2]) :
    m_center(center)
{
    m_axis[0] = axis[0];
    m_axis[1] = axis[1];
    m_extent[0] = extent[0];
    m_extent[1] = extent[1];
}

Box2::Box2(const Vector2& center, const Vector2& axis0, const Vector2& axis1, float extent0, float extent1) :
    m_center(center)
{
    m_axis[0] = axis0;
    m_axis[1] = axis1;
    m_extent[0] = extent0;
    m_extent[1] = extent1;
}

std::array<Vector2, 4> Box2::ComputeVertices() const
{
    Vector2 extent_axis[2] =
    {
        m_axis[0] * m_extent[0],
        m_axis[1] * m_extent[1]
    };

    std::array<Vector2, 4> vertex;
    vertex[0] = m_center - extent_axis[0] - extent_axis[1];
    vertex[1] = m_center + extent_axis[0] - extent_axis[1];
    vertex[2] = m_center + extent_axis[0] + extent_axis[1];
    vertex[3] = m_center - extent_axis[0] + extent_axis[1];
    return vertex;
}

bool Box2::operator ==(const Box2& box) const
{
    return ((m_center == box.m_center) && (m_axis[0] == box.m_axis[0]) && (m_axis[1] == box.m_axis[1])
        && (Math::IsEqual(m_extent[0], box.m_extent[0])) && (Math::IsEqual(m_extent[1], box.m_extent[1])));
}

bool Box2::operator !=(const Box2& box) const
{
    return ((m_center != box.m_center) || (m_axis[0] != box.m_axis[0]) || (m_axis[1] != box.m_axis[1])
        || (!Math::IsEqual(m_extent[0], box.m_extent[0])) || (!Math::IsEqual(m_extent[1], box.m_extent[1])));
}

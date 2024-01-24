#include "Box3.h"
#include "MathGlobal.h"
#include <cassert>

using namespace Enigma::MathLib;

const Box3 Box3::UNIT_BOX(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f, 1.0f);

Box3::Box3()
{
    m_axis[0] = Vector3::UNIT_X;
    m_axis[1] = Vector3::UNIT_Y;
    m_axis[2] = Vector3::UNIT_Z;
    m_center = Vector3::ZERO;
    m_extent[0] = m_extent[1] = m_extent[2] = 0.0f;
}

Box3::Box3(const Vector3& center, const Vector3 axis[3], const float extent[3]) :
    m_center(center)
{
    assert((extent[0] > 0.0f) && (extent[1] > 0.0f) && (extent[2] > 0.0f));
    m_axis[0] = axis[0];
    m_axis[1] = axis[1];
    m_axis[2] = axis[2];
    m_extent[0] = extent[0];
    m_extent[1] = extent[1];
    m_extent[2] = extent[2];
}

Box3::Box3(const Vector3& center, const Vector3& axis0, const Vector3& axis1, const Vector3& axis2, float extent0,
    float extent1, float extent2) :
    m_center(center)
{
    assert((extent0 > 0.0f) && (extent1 > 0.0f) && (extent2 > 0.0f));
    m_axis[0] = axis0;
    m_axis[1] = axis1;
    m_axis[2] = axis2;
    m_extent[0] = extent0;
    m_extent[1] = extent1;
    m_extent[2] = extent2;
}

std::array<Vector3, 8> Box3::ComputeVertices() const
{
    Vector3 extent_axis[3] =
    {
        m_extent[0] * m_axis[0],
        m_extent[1] * m_axis[1],
        m_extent[2] * m_axis[2]
    };

    std::array<Vector3, 8> vertex;
    vertex[0] = m_center - extent_axis[0] - extent_axis[1] - extent_axis[2];
    vertex[1] = m_center + extent_axis[0] - extent_axis[1] - extent_axis[2];
    vertex[2] = m_center + extent_axis[0] + extent_axis[1] - extent_axis[2];
    vertex[3] = m_center - extent_axis[0] + extent_axis[1] - extent_axis[2];
    vertex[4] = m_center - extent_axis[0] - extent_axis[1] + extent_axis[2];
    vertex[5] = m_center + extent_axis[0] - extent_axis[1] + extent_axis[2];
    vertex[6] = m_center + extent_axis[0] + extent_axis[1] + extent_axis[2];
    vertex[7] = m_center - extent_axis[0] + extent_axis[1] + extent_axis[2];
    return vertex;
}

bool Box3::operator ==(const Box3& box) const
{
    return ((m_center == box.m_center) && (m_axis[0] == box.m_axis[0]) && (m_axis[1] == box.m_axis[1])
        && (m_axis[2] == box.m_axis[2])
        && (Math::IsEqual(m_extent[0], box.m_extent[0])) && (Math::IsEqual(m_extent[1], box.m_extent[1]))
        && (Math::IsEqual(m_extent[2], box.m_extent[2])));
}

bool Box3::operator !=(const Box3& box) const
{
    return ((m_center != box.m_center) || (m_axis[0] != box.m_axis[0]) || (m_axis[1] != box.m_axis[1])
        || (m_axis[2] != box.m_axis[2])
        || (!Math::IsEqual(m_extent[0], box.m_extent[0])) || (!Math::IsEqual(m_extent[1], box.m_extent[1]))
        || (!Math::IsEqual(m_extent[2], box.m_extent[2])));
}

bool Box3::isEmpty() const
{
    if ((m_extent[0] <= 0.0f) || (m_extent[1] <= 0.0f) || (m_extent[2] <= 0.0f)) return true;
    return false;
}

Box3 Box3::SwapToMajorAxis() const
{
    Box3 box(*this);
    box.ImplicitSwapToMajorAxis();
    return box;
}

void Box3::ImplicitSwapToMajorAxis()
{
    // 將m_axis 0,1,2調整到主要為x,y,z
    if (m_axis[1].x() * m_axis[1].x() > m_axis[0].x() * m_axis[0].x())
    {
        // y <--> x
        float extend_save;
        Vector3 axis_save;
        extend_save = m_extent[0];
        m_extent[0] = m_extent[1];
        m_extent[1] = extend_save;
        axis_save = m_axis[0];
        m_axis[0] = m_axis[1];
        m_axis[1] = axis_save;
    }
    if (m_axis[2].x() * m_axis[2].x() > m_axis[0].x() * m_axis[0].x())
    {
        // z <--> x
        float extend_save;
        Vector3 axis_save;
        extend_save = m_extent[0];
        m_extent[0] = m_extent[2];
        m_extent[2] = extend_save;
        axis_save = m_axis[0];
        m_axis[0] = m_axis[2];
        m_axis[2] = axis_save;
    }
    if (m_axis[2].y() * m_axis[2].y() > m_axis[1].y() * m_axis[1].y())
    {
        // z <--> y
        float extend_save;
        Vector3 axis_save;
        extend_save = m_extent[1];
        m_extent[1] = m_extent[2];
        m_extent[2] = extend_save;
        axis_save = m_axis[1];
        m_axis[1] = m_axis[2];
        m_axis[2] = axis_save;
    }
    // 把軸都調整到正向
    if (m_axis[0].x() < 0.0f) m_axis[0] = -m_axis[0];
    if (m_axis[1].y() < 0.0f) m_axis[1] = -m_axis[1];
    if (m_axis[2].z() < 0.0f) m_axis[2] = -m_axis[2];
    // 調整正交規則
    Vector3 cross = m_axis[0].cross(m_axis[1]);
    if (cross.dot(m_axis[2]) < 0.0f)
    {
        m_axis[0] = -m_axis[0];
    }
}

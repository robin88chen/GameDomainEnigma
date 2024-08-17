﻿#include "IntrGeometryRay3.h"
#include "GameEngine/IntrBVRay3.h"
#include "IntrGeometryCache.h"
#include "TriangleList.h"
#include "MathLib/IntrRay3Triangle3.h"
#include "Frameworks/unique_ptr_dynamic_cast.hpp"
#include <cassert>

using namespace Enigma::Geometries;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

IntrGeometryRay3::IntrGeometryRay3(const GeometryDataPtr& geo, const Ray3& ray) :
    Intersector(), m_ray(ray), m_geometry(geo)
{
    m_intersectionType = IntersectionType::POINT;
}

IntrGeometryRay3::~IntrGeometryRay3()
{
    m_geometry = nullptr;
}

const Ray3& IntrGeometryRay3::ray() const
{
    return m_ray;
}

const GeometryDataPtr& IntrGeometryRay3::geometry()
{
    return m_geometry;
}

Intersector::Result IntrGeometryRay3::test(std::unique_ptr<IntersectorCache> last_result)
{
    if (!m_geometry) return { false, std::move(last_result) };
    // test bounding volume first
    if (!m_geometry->getBoundingVolume().isEmpty())
    {
        IntrBVRay3 intr_bv(m_geometry->getBoundingVolume(), m_ray);
        auto res = intr_bv.test(nullptr);
        if (!res.m_hasIntersect) return { false, std::move(last_result) };
    }

    // test for triangle list
    if (m_geometry->getPrimitiveTopology() == Graphics::PrimitiveTopology::Topology_TriangleList)
    {
        std::unique_ptr<IntrGeometryCache> geo_cache = nullptr;
        if (last_result)
        {
            geo_cache = stdext::dynamic_pointer_cast<IntrGeometryCache>(std::move(last_result));
        }
        return testForTriangleList(std::move(geo_cache));
    }
    return { false, std::move(last_result) };
}

Intersector::Result IntrGeometryRay3::find(std::unique_ptr<IntersectorCache> last_result)
{
    if (!m_geometry) return { false, std::move(last_result) };
    // test bounding volume first
    if (!m_geometry->getBoundingVolume().isEmpty())
    {
        IntrBVRay3 intr_bv(m_geometry->getBoundingVolume(), m_ray);
        auto res = intr_bv.test(nullptr);
        if (!res.m_hasIntersect) return { false, std::move(last_result) };
    }

    // test for triangle list
    if (m_geometry->getPrimitiveTopology() == Graphics::PrimitiveTopology::Topology_TriangleList)
    {
        std::unique_ptr<IntrGeometryCache> geo_cache = nullptr;
        if (last_result)
        {
            geo_cache = stdext::dynamic_pointer_cast<IntrGeometryCache>(std::move(last_result));
        }
        return findForTriangleList(std::move(geo_cache));
    }
    return { false, std::move(last_result) };
}

size_t IntrGeometryRay3::getQuantity() const
{
    return m_points.size();
}

const Vector3& IntrGeometryRay3::getPoint(unsigned int i) const
{
    assert(i < m_points.size());
    return m_points[i];
}

float IntrGeometryRay3::getRayT(unsigned int i) const
{
    assert(i < m_tParams.size());
    return m_tParams[i];
}

const std::vector<Vector3>& IntrGeometryRay3::getPointArray() const
{
    return m_points;
}

const std::vector<float>& IntrGeometryRay3::getParamArray() const
{
    return m_tParams;
}

Intersector::Result IntrGeometryRay3::testForTriangleList(std::unique_ptr<IntrGeometryCache> geo_cache)
{
    TriangleListPtr tri_list = std::dynamic_pointer_cast<TriangleList, GeometryData>(m_geometry);
    if (!tri_list) return { false, std::move(geo_cache) };

    unsigned int tri_count = tri_list->getTriangleCount();
    if (tri_count <= 0) return { false, std::move(geo_cache) };

    unsigned int start_index = 0;
    if (geo_cache) start_index = geo_cache->getElementCachedIndex();
    unsigned int forward_count = tri_count - start_index;
    unsigned int backward_count = start_index + 1;
    unsigned int test_total = forward_count * 2;
    if (backward_count > forward_count) test_total = backward_count * 2;

    Vector3 triangle[3];
    for (unsigned int i = 1; i < test_total; i++) // 從1開始，加減0是一樣的
    {
        int tri_index;
        if (i & 0x01)
        {
            tri_index = start_index + (i >> 1);
        }
        else
        {
            tri_index = start_index - (i >> 1);
        }
        if ((tri_index < 0) || (tri_index >= (int)tri_count)) continue;

        tri_list->fetchTrianglePos(tri_index, triangle);
        if (!IntrRay3Triangle3(m_ray, triangle).test(nullptr).m_hasIntersect) continue;

        // if we got here, we found intersection
        if (geo_cache == nullptr) geo_cache = std::make_unique<IntrGeometryCache>();
        if (geo_cache) geo_cache->setElementCachedIndex(tri_index);
        return { true, std::move(geo_cache) };
    }

    return { false, std::move(geo_cache) };
}

Intersector::Result IntrGeometryRay3::findForTriangleList(std::unique_ptr<IntrGeometryCache> geo_cache)
{
    m_tParams.clear();
    m_points.clear();

    TriangleListPtr tri_list = std::dynamic_pointer_cast<TriangleList, GeometryData>(m_geometry);
    if (!tri_list) return { false, std::move(geo_cache) };

    unsigned int tri_count = tri_list->getTriangleCount();
    if (tri_count <= 0) return { false, std::move(geo_cache) };

    unsigned int start_index = 0;
    if (geo_cache) start_index = geo_cache->getElementCachedIndex();
    unsigned int forward_count = tri_count - start_index;
    unsigned int backward_count = start_index + 1;
    unsigned int test_total = forward_count * 2;
    if (backward_count > forward_count) test_total = backward_count * 2;
    unsigned int req_result_total = 0xffffffff;
    if ((geo_cache) && (geo_cache->getRequiredResultCount())) req_result_total = geo_cache->getRequiredResultCount();
    unsigned int result_count = 0;

    Vector3 triangle[3];
    for (unsigned int i = 1; i < test_total; i++) // 從1開始，加減0是一樣的
    {
        int tri_index;
        if (i & 0x01)
        {
            tri_index = start_index + (i >> 1);
        }
        else
        {
            tri_index = start_index - (i >> 1);
        }
        if ((tri_index < 0) || (tri_index >= (int)tri_count)) continue;

        tri_list->fetchTrianglePos(tri_index, triangle);
        IntrRay3Triangle3 intr(m_ray, triangle);
        if (!intr.find(nullptr).m_hasIntersect) continue;
        float t = intr.getRayT();
        if (t < 0.0f) continue;

        // if we got here, we found intersection
        m_tParams.emplace_back(t);
        if (geo_cache == nullptr) geo_cache = std::make_unique<IntrGeometryCache>();
        if (geo_cache) geo_cache->setElementCachedIndex(tri_index);
        result_count++;
        if (result_count >= req_result_total) break;
    }

    if (m_tParams.empty()) return { false, std::move(geo_cache) };
    sort(m_tParams.begin(), m_tParams.end());

    m_points.resize(m_tParams.size());
    for (unsigned int i = 0; i < m_tParams.size(); i++)
    {
        m_points[i] = m_tParams[i] * m_ray.direction() + m_ray.origin();
    }

    return { true, std::move(geo_cache) };
}

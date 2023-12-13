#include "MeshPrimitiveRay3IntersectionFinder.h"
#include "GameEngine/IntersectionFinderFactories.h"
#include "Geometries/IntrGeometryRay3.h"
#include "Geometries/IntrGeometryCache.h"
#include "MeshPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Geometries;
using namespace Enigma::MathLib;

MeshPrimitiveRay3IntersectionFinder::MeshPrimitiveRay3IntersectionFinder()
{

}

MeshPrimitiveRay3IntersectionFinder::~MeshPrimitiveRay3IntersectionFinder()
{

}

PrimitiveRay3IntersectionFinder* MeshPrimitiveRay3IntersectionFinder::create()
{
    return new MeshPrimitiveRay3IntersectionFinder();
}

void MeshPrimitiveRay3IntersectionFinder::RegisterFactory()
{
    PrimitiveRay3IntersectionFinderFactory::RegisterCreator(MeshPrimitive::TYPE_RTTI.getName(), create);
}

Intersector::Result MeshPrimitiveRay3IntersectionFinder::Test(const std::shared_ptr<Primitive>& primitive, const Ray3& ray,
    std::unique_ptr<IntersectorCache> cache) const
{
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(primitive);
    if (!mesh) return { false, std::move(cache) };
    if (cache == nullptr)
    {
        auto geo_cache = std::make_unique<IntrGeometryCache>();
        geo_cache->SetRequiredResultCount(m_requiredResultCount);
        cache = std::move(geo_cache);
    }
    return TestMesh(mesh, ray, std::move(cache));
}

std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, Intersector::Result> MeshPrimitiveRay3IntersectionFinder::Find(const std::shared_ptr<Primitive>& primitive, const Ray3& ray, std::unique_ptr<IntersectorCache> cache) const
{
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(primitive);
    if (!mesh) return { std::vector<IntrPrimitiveRay3::ResultRecord>{}, Intersector::Result(false, std::move(cache)) };
    if (cache == nullptr)
    {
        auto geo_cache = std::make_unique<IntrGeometryCache>();
        geo_cache->SetRequiredResultCount(m_requiredResultCount);
        cache = std::move(geo_cache);
    }
    return FindMesh(mesh, ray, std::move(cache));
}

Intersector::Result MeshPrimitiveRay3IntersectionFinder::TestMesh(const std::shared_ptr<MeshPrimitive>& mesh,
    const Ray3& point_ray, std::unique_ptr<IntersectorCache> cache) const
{
    Ray3 ray = point_ray;
    Matrix4 inv_world = mesh->getPrimitiveWorldTransform().Inverse();
    ray.Origin() = inv_world.TransformCoord(ray.Origin());
    std::tie(ray.Direction(), std::ignore) = inv_world.TransformVectorNormalized(ray.Direction());

    IntrGeometryRay3 intr_geo(mesh->getGeometryData(), ray);
    return intr_geo.Test(std::move(cache));
}

std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, Intersector::Result> MeshPrimitiveRay3IntersectionFinder::FindMesh(
    const std::shared_ptr<MeshPrimitive>& mesh, const Ray3& point_ray, std::unique_ptr<IntersectorCache> cache) const
{
    Ray3 ray = point_ray;
    Matrix4 inv_world = mesh->getPrimitiveWorldTransform().Inverse();
    float dir_length;
    ray.Origin() = inv_world.TransformCoord(ray.Origin());
    std::tie(ray.Direction(), dir_length) = inv_world.TransformVectorNormalized(ray.Direction());

    std::vector<IntrPrimitiveRay3::ResultRecord> records;
    IntrGeometryRay3 intr_geo(mesh->getGeometryData(), ray);
    auto res = intr_geo.Find(std::move(cache));
    if (res.m_hasIntersect)
    {
        for (unsigned int i = 0; i < intr_geo.GetQuantity(); i++)
        {
            float t = intr_geo.GetRayT(i) / dir_length;
            records.emplace_back(IntrPrimitiveRay3::ResultRecord(t, t * point_ray.Direction() + point_ray.Origin(), mesh));
        }
    }
    return { records, Intersector::Result(res.m_hasIntersect, std::move(res.m_cache)) };
}

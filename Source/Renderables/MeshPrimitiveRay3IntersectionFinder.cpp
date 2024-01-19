#include "MeshPrimitiveRay3IntersectionFinder.h"
#include "Primitives/PrimitiveRay3IntersectionFinder.h"
#include "Primitives/PrimitiveIntersectionFinderFactories.h"
#include "Geometries/IntrGeometryRay3.h"
#include "Geometries/IntrGeometryCache.h"
#include "MeshPrimitive.h"

using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::Geometries;
using namespace Enigma::MathLib;
using namespace Enigma::Primitives;

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
    PrimitiveRay3IntersectionFinderFactory::registerCreator(MeshPrimitive::TYPE_RTTI.getName(), create);
}

Intersector::Result MeshPrimitiveRay3IntersectionFinder::test(const std::shared_ptr<Primitive>& primitive, const Ray3& ray,
    std::unique_ptr<IntersectorCache> cache) const
{
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(primitive);
    if (!mesh) return { false, std::move(cache) };
    if (cache == nullptr)
    {
        auto geo_cache = std::make_unique<IntrGeometryCache>();
        geo_cache->setRequiredResultCount(m_requiredResultCount);
        cache = std::move(geo_cache);
    }
    return testMesh(mesh, ray, std::move(cache));
}

std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, Intersector::Result> MeshPrimitiveRay3IntersectionFinder::find(const std::shared_ptr<Primitive>& primitive, const Ray3& ray, std::unique_ptr<IntersectorCache> cache) const
{
    auto mesh = std::dynamic_pointer_cast<MeshPrimitive>(primitive);
    if (!mesh) return { std::vector<IntrPrimitiveRay3::ResultRecord>{}, Intersector::Result(false, std::move(cache)) };
    if (cache == nullptr)
    {
        auto geo_cache = std::make_unique<IntrGeometryCache>();
        geo_cache->setRequiredResultCount(m_requiredResultCount);
        cache = std::move(geo_cache);
    }
    return findMesh(mesh, ray, std::move(cache));
}

Intersector::Result MeshPrimitiveRay3IntersectionFinder::testMesh(const std::shared_ptr<MeshPrimitive>& mesh,
    const Ray3& point_ray, std::unique_ptr<IntersectorCache> cache) const
{
    Ray3 ray = point_ray;
    Matrix4 inv_world = mesh->getPrimitiveWorldTransform().Inverse();
    ray.origin() = inv_world.TransformCoord(ray.origin());
    std::tie(ray.direction(), std::ignore) = inv_world.TransformVectorNormalized(ray.direction());

    IntrGeometryRay3 intr_geo(mesh->getGeometryData(), ray);
    return intr_geo.test(std::move(cache));
}

std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, Intersector::Result> MeshPrimitiveRay3IntersectionFinder::findMesh(
    const std::shared_ptr<MeshPrimitive>& mesh, const Ray3& point_ray, std::unique_ptr<IntersectorCache> cache) const
{
    Ray3 ray = point_ray;
    Matrix4 inv_world = mesh->getPrimitiveWorldTransform().Inverse();
    float dir_length;
    ray.origin() = inv_world.TransformCoord(ray.origin());
    std::tie(ray.direction(), dir_length) = inv_world.TransformVectorNormalized(ray.direction());

    std::vector<IntrPrimitiveRay3::ResultRecord> records;
    IntrGeometryRay3 intr_geo(mesh->getGeometryData(), ray);
    auto res = intr_geo.find(std::move(cache));
    if (res.m_hasIntersect)
    {
        for (unsigned int i = 0; i < intr_geo.getQuantity(); i++)
        {
            float t = intr_geo.getRayT(i) / dir_length;
            records.emplace_back(IntrPrimitiveRay3::ResultRecord(t, t * point_ray.direction() + point_ray.origin(), mesh));
        }
    }
    return { records, Intersector::Result(res.m_hasIntersect, std::move(res.m_cache)) };
}

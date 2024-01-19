#include "ModelPrimitiveRay3IntersectionFinder.h"
#include "Primitives/PrimitiveIntersectionFinderFactories.h"
#include "ModelPrimitive.h"
#include "IntrModelPrimitiveCache.h"
#include "MeshPrimitiveRay3IntersectionFinder.h"
#include "Frameworks/unique_ptr_dynamic_cast.hpp"
#include "MeshPrimitive.h"

using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Primitives;

ModelPrimitiveRay3IntersectionFinder::ModelPrimitiveRay3IntersectionFinder()
{

}

ModelPrimitiveRay3IntersectionFinder::~ModelPrimitiveRay3IntersectionFinder()
{

}

PrimitiveRay3IntersectionFinder* ModelPrimitiveRay3IntersectionFinder::create()
{
    return new ModelPrimitiveRay3IntersectionFinder();
}

void ModelPrimitiveRay3IntersectionFinder::registerFactory()
{
    PrimitiveRay3IntersectionFinderFactory::registerCreator(ModelPrimitive::TYPE_RTTI.getName(), create);
}

Intersector::Result ModelPrimitiveRay3IntersectionFinder::test(const std::shared_ptr<Primitive>& primitive, const Ray3& ray,
    std::unique_ptr<IntersectorCache> cache) const
{
    auto model = std::dynamic_pointer_cast<ModelPrimitive>(primitive);
    if (!model) return { false, std::move(cache) };
    if (cache == nullptr)
    {
        auto model_cache = std::make_unique<IntrModelPrimitiveCache>();
        model_cache->setRequiredResultCount(m_requiredResultCount);
        cache = std::move(model_cache);
    }
    return testModel(model, ray, std::move(cache));
}

std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, Intersector::Result> ModelPrimitiveRay3IntersectionFinder::find(const std::shared_ptr<
    Primitive>& primitive, const Ray3& ray, std::unique_ptr<IntersectorCache> cache) const
{
    auto model = std::dynamic_pointer_cast<ModelPrimitive>(primitive);
    if (!model) return { std::vector<IntrPrimitiveRay3::ResultRecord>{}, Intersector::Result(false, std::move(cache)) };
    if (cache == nullptr)
    {
        auto model_cache = std::make_unique<IntrModelPrimitiveCache>();
        model_cache->setRequiredResultCount(m_requiredResultCount);
        cache = std::move(model_cache);
    }
    return findModel(model, ray, std::move(cache));
}

Intersector::Result ModelPrimitiveRay3IntersectionFinder::testModel(const std::shared_ptr<ModelPrimitive>& model,
    const Ray3& ray, std::unique_ptr<IntersectorCache> cache) const
{
    if (!model) return { false, std::move(cache) };
    unsigned int mesh_count = model->getMeshPrimitiveCount();
    if (mesh_count <= 0) return { false, std::move(cache) };

    std::unique_ptr<IntrModelPrimitiveCache> prim_cache = nullptr;
    if (cache) prim_cache = stdext::dynamic_pointer_cast<IntrModelPrimitiveCache>(std::move(cache));
    // check cached mesh index first
    unsigned int mesh_cache_index = 0xffffffff;
    if (prim_cache) mesh_cache_index = prim_cache->getCachedMeshPrimIndex();
    if (mesh_cache_index < mesh_count)
    {
        if (auto mesh_prim = model->getMeshPrimitive(mesh_cache_index))
        {
            MeshPrimitiveRay3IntersectionFinder mesh_finder;
            auto res = mesh_finder.test(mesh_prim, ray, prim_cache->getIntrGeometryCache());
            if (res.m_hasIntersect)
            {
                if (prim_cache)
                {
                    prim_cache->setCachedMeshPrimIndex(mesh_cache_index);
                    prim_cache->setIntrGeometryCache(std::move(res.m_cache));
                }
                return { res.m_hasIntersect, std::move(prim_cache) };
            }
        }
    }
    std::unique_ptr<IntersectorCache> geo_cache = nullptr;
    if (prim_cache) geo_cache = prim_cache->getIntrGeometryCache();
    for (unsigned int i = 0; i < mesh_count; i++)
    {
        if (i == mesh_cache_index) continue;
        std::shared_ptr<MeshPrimitive> mesh = model->getMeshPrimitive(i);
        if (!mesh) continue;
        MeshPrimitiveRay3IntersectionFinder mesh_finder;
        auto res = mesh_finder.test(mesh, ray, std::move(geo_cache));
        if (res.m_hasIntersect)
        {
            if (prim_cache == nullptr) prim_cache = std::make_unique<IntrModelPrimitiveCache>();
            if (prim_cache)
            {
                prim_cache->setCachedMeshPrimIndex(i);
                prim_cache->setIntrGeometryCache(std::move(res.m_cache));
            }
            return { res.m_hasIntersect, std::move(prim_cache) };
        }
    }

    return { false, std::move(prim_cache) };
}

std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, Intersector::Result> ModelPrimitiveRay3IntersectionFinder::findModel(const std::shared_ptr<ModelPrimitive>& model, const
    Ray3& ray, std::unique_ptr<IntersectorCache> cache) const
{
    std::vector<IntrPrimitiveRay3::ResultRecord> records;

    if (!model) return { records , Intersector::Result{ false, std::move(cache) } };
    unsigned int mesh_count = model->getMeshPrimitiveCount();
    if (mesh_count <= 0) return { records , Intersector::Result{ false, std::move(cache) } };

    std::unique_ptr<IntrModelPrimitiveCache> prim_cache = nullptr;
    if (cache) prim_cache = stdext::dynamic_pointer_cast<IntrModelPrimitiveCache>(std::move(cache));
    unsigned int req_result_total = 0xffffffff;
    if ((prim_cache) && (prim_cache->getRequiredResultCount())) req_result_total = prim_cache->getRequiredResultCount();
    unsigned int result_count = 0;

    // check cached mesh index first
    unsigned int mesh_cache_index = 0xffffffff;
    if (prim_cache) mesh_cache_index = prim_cache->getCachedMeshPrimIndex();
    if (mesh_cache_index < mesh_count)
    {
        if (std::shared_ptr<MeshPrimitive> mesh = model->getMeshPrimitive(mesh_cache_index))
        {
            MeshPrimitiveRay3IntersectionFinder mesh_finder;
            auto [recs, res] = mesh_finder.find(mesh, ray, prim_cache->getIntrGeometryCache());
            if (res.m_hasIntersect)
            {
                if (prim_cache)
                {
                    prim_cache->setCachedMeshPrimIndex(mesh_cache_index);
                    prim_cache->setIntrGeometryCache(std::move(res.m_cache));
                }
                result_count += static_cast<unsigned int>(recs.size());
                records.insert(records.end(), recs.begin(), recs.end());
            }
        }
    }
    std::unique_ptr<IntersectorCache> geo_cache = nullptr;
    if (prim_cache) geo_cache = prim_cache->getIntrGeometryCache();
    for (unsigned int m = 0; m < mesh_count; m++)
    {
        if (result_count >= req_result_total) break;
        if (m == mesh_cache_index) continue;
        std::shared_ptr<MeshPrimitive> mesh = model->getMeshPrimitive(m);
        if (!mesh) continue;

        MeshPrimitiveRay3IntersectionFinder mesh_finder;
        auto [recs, res] = mesh_finder.find(mesh, ray, std::move(geo_cache));
        if (res.m_hasIntersect)
        {
            if (prim_cache == nullptr) prim_cache = std::make_unique<IntrModelPrimitiveCache>();
            if (prim_cache)
            {
                prim_cache->setCachedMeshPrimIndex(m);
                prim_cache->setIntrGeometryCache(std::move(res.m_cache));
            }
            result_count += static_cast<unsigned int>(recs.size());
            records.insert(records.end(), recs.begin(), recs.end());
        }
    }
    if (records.empty()) return { records, Intersector::Result{ false, std::move(prim_cache) } };

    return { records, Intersector::Result{ true, std::move(prim_cache) } };
}


#include "ModelPrimitiveRay3IntersectionFinder.h"
#include "GameEngine/IntersectionFinderFactories.h"
#include "ModelPrimitive.h"
#include "IntrModelPrimitiveCache.h"
#include "MeshPrimitiveRay3IntersectionFinder.h"
#include "Platforms/MemoryAllocMacro.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

ModelPrimitiveRay3IntersectionFinder::ModelPrimitiveRay3IntersectionFinder()
{

}

ModelPrimitiveRay3IntersectionFinder::~ModelPrimitiveRay3IntersectionFinder()
{

}

PrimitiveRay3IntersectionFinder* ModelPrimitiveRay3IntersectionFinder::Create()
{
    return new ModelPrimitiveRay3IntersectionFinder();
}

void ModelPrimitiveRay3IntersectionFinder::RegisterFactory()
{
    PrimitiveRay3IntersectionFinderFactory::RegisterCreator(ModelPrimitive::TYPE_RTTI.GetName(), Create);
}

Intersector::Result ModelPrimitiveRay3IntersectionFinder::Test(const std::shared_ptr<Primitive>& primitive, const Ray3& ray,
                                                               IntersectorCache* cache) const
{
    auto model = std::dynamic_pointer_cast<ModelPrimitive>(primitive);
    if (!model) return { false, cache };
    return TestModel(model, ray, cache);
}

std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, Intersector::Result> ModelPrimitiveRay3IntersectionFinder::Find(const std::shared_ptr<
    Primitive>& primitive, const Ray3& ray, IntersectorCache* cache) const
{
    auto model = std::dynamic_pointer_cast<ModelPrimitive>(primitive);
    if (!model) return { {}, { false, cache } };
    return FindModel(model, ray, cache);
}

Intersector::Result ModelPrimitiveRay3IntersectionFinder::TestModel(const std::shared_ptr<ModelPrimitive>& model,
    const Ray3& ray, IntersectorCache* cache) const
{
    if (!model) return { false, cache };
    unsigned int mesh_count = model->GetMeshPrimitiveCount();
    if (mesh_count <= 0) return { false, cache };

    IntrModelPrimitiveCache* prim_cache = nullptr;
    if (cache) prim_cache = dynamic_cast<IntrModelPrimitiveCache*>(cache);
    // check cached mesh index first
    unsigned int mesh_cache_index = 0xffffffff;
    if (prim_cache) mesh_cache_index = prim_cache->GetCachedMeshPrimIndex();
    if (mesh_cache_index < mesh_count)
    {
        if (auto mesh_prim = model->GetMeshPrimitive(mesh_cache_index))
        {
            MeshPrimitiveRay3IntersectionFinder mesh_finder;
            auto res = mesh_finder.Test(mesh_prim, ray, prim_cache->GetIntrGeometryCache());
            if (res.m_hasIntersect)
            {
                if (prim_cache) prim_cache->SetCachedMeshPrimIndex(mesh_cache_index);
                return { res.m_hasIntersect, prim_cache };
            }
        }
    }
    IntersectorCache* geo_cache = nullptr;
    if (prim_cache) geo_cache = prim_cache->GetIntrGeometryCache();
    for (unsigned int i = 0; i < mesh_count; i++)
    {
        if (i == mesh_cache_index) continue;
        MeshPrimitivePtr mesh = model->GetMeshPrimitive(i);
        if (!mesh) continue;
        MeshPrimitiveRay3IntersectionFinder mesh_finder;
        auto res = mesh_finder.Test(mesh, ray, geo_cache);
        if (res.m_hasIntersect)
        {
            if (prim_cache == nullptr) prim_cache = menew IntrModelPrimitiveCache();
            if (prim_cache)
            {
                prim_cache->SetCachedMeshPrimIndex(i);
                prim_cache->SetIntrGeometryCache(res.m_cache);
            }
            return { res.m_hasIntersect, prim_cache };
        }
    }

    return { false, cache };
}

std::tuple<std::vector<IntrPrimitiveRay3::ResultRecord>, Intersector::Result> ModelPrimitiveRay3IntersectionFinder::FindModel(const std::shared_ptr<ModelPrimitive>& model, const
    Ray3& ray, IntersectorCache* cache) const
{
    std::vector<IntrPrimitiveRay3::ResultRecord> records;

    if (!model) return { {}, { false, cache } };
    unsigned int mesh_count = model->GetMeshPrimitiveCount();
    if (mesh_count <= 0) return { {}, { false, cache } };

    IntrModelPrimitiveCache* prim_cache = nullptr;
    if (cache) prim_cache = dynamic_cast<IntrModelPrimitiveCache*>(cache);
    unsigned int req_result_total = 0xffffffff;
    if ((prim_cache) && (prim_cache->GetRequiredResultCount())) req_result_total = prim_cache->GetRequiredResultCount();
    unsigned int result_count = 0;

    // check cached mesh index first
    unsigned int mesh_cache_index = 0xffffffff;
    if (prim_cache) mesh_cache_index = prim_cache->GetCachedMeshPrimIndex();
    if (mesh_cache_index < mesh_count)
    {
        if (MeshPrimitivePtr mesh = model->GetMeshPrimitive(mesh_cache_index))
        {
            MeshPrimitiveRay3IntersectionFinder mesh_finder;
            auto [recs, res] = mesh_finder.Find(mesh, ray, prim_cache->GetIntrGeometryCache());
            if (res.m_hasIntersect)
            {
                if (prim_cache) prim_cache->SetCachedMeshPrimIndex(mesh_cache_index);
                result_count += static_cast<unsigned int>(recs.size());
                records.insert(records.end(), recs.begin(), recs.end());
            }
        }
    }
    IntersectorCache* geo_cache = nullptr;
    if (prim_cache) geo_cache = prim_cache->GetIntrGeometryCache();
    for (unsigned int m = 0; m < mesh_count; m++)
    {
        if (result_count >= req_result_total) break;
        if (m == mesh_cache_index) continue;
        MeshPrimitivePtr mesh = model->GetMeshPrimitive(m);
        if (!mesh) continue;

        MeshPrimitiveRay3IntersectionFinder mesh_finder;
        auto [recs, res] = mesh_finder.Find(mesh, ray, geo_cache);
        if (res.m_hasIntersect)
        {
            if (prim_cache == nullptr) prim_cache = menew IntrModelPrimitiveCache();
            if (prim_cache)
            {
                prim_cache->SetCachedMeshPrimIndex(m);
                prim_cache->SetIntrGeometryCache(res.m_cache);
            }
            result_count += static_cast<unsigned int>(recs.size());
            records.insert(records.end(), recs.begin(), recs.end());
        }
    }
    if (records.empty()) return { {}, { false, prim_cache } };

    return { records, { true, prim_cache } };
}


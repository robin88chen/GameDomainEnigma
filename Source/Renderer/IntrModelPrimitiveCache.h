/*********************************************************************
 * \file   IntrModelPrimitiveCache.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef INTR_MODEL_PRIMITIVE_CACHE_H
#define INTR_MODEL_PRIMITIVE_CACHE_H

#include "MathLib/IntersectorCache.h"
#include "Geometries/IntrGeometryCache.h"
#include "Frameworks/unique_ptr_dynamic_cast.hpp"

namespace Enigma::Renderer
{
    class IntrModelPrimitiveCache : public MathLib::IntersectorCache
    {
    public:
        IntrModelPrimitiveCache() : m_geoCache(nullptr), m_meshPrimIndex{ 0 }, m_requiredResultCount{ 0 } {};
        ~IntrModelPrimitiveCache() {}

        unsigned int GetCachedMeshPrimIndex() { return m_meshPrimIndex; };
        void SetCachedMeshPrimIndex(unsigned int index) { m_meshPrimIndex = index; };
        std::unique_ptr<MathLib::IntersectorCache> GetIntrGeometryCache() { return std::move(m_geoCache); };
        void SetIntrGeometryCache(std::unique_ptr<MathLib::IntersectorCache> cache) { m_geoCache = stdext::dynamic_pointer_cast<Geometries::IntrGeometryCache>(std::move(cache)); };
        unsigned int GetRequiredResultCount() { return m_requiredResultCount; };
        void SetRequiredResultCount(unsigned int count)
        {
            m_requiredResultCount = count; if (m_geoCache) m_geoCache->SetRequiredResultCount(count);
        };
    private:
        std::unique_ptr<Geometries::IntrGeometryCache> m_geoCache;
        unsigned int m_meshPrimIndex;
        unsigned int m_requiredResultCount;
    };
};

#endif // INTR_MODEL_PRIMITIVE_CACHE_H

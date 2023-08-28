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
#include "GameEngine/IntrGeometryCache.h"

namespace Enigma::Renderer
{
    class IntrModelPrimitiveCache : public MathLib::IntersectorCache
    {
    public:
        IntrModelPrimitiveCache() : m_geoCache(nullptr), m_meshPrimIndex{ 0 }, m_requiredResultCount{ 0 } {};
        ~IntrModelPrimitiveCache() { if (m_geoCache) delete m_geoCache; }

        unsigned int GetCachedMeshPrimIndex() { return m_meshPrimIndex; };
        void SetCachedMeshPrimIndex(unsigned int index) { m_meshPrimIndex = index; };
        MathLib::IntersectorCache* GetIntrGeometryCache() { return m_geoCache; };
        void SetIntrGeometryCache(MathLib::IntersectorCache* cache) { m_geoCache = dynamic_cast<Engine::IntrGeometryCache*>(cache); };
        unsigned int GetRequiredResultCount() { return m_requiredResultCount; };
        void SetRequiredResultCount(unsigned int count)
        {
            m_requiredResultCount = count; if (m_geoCache) m_geoCache->SetRequiredResultCount(count);
        };
    private:
        Engine::IntrGeometryCache* m_geoCache;
        unsigned int m_meshPrimIndex;
        unsigned int m_requiredResultCount;
    };
};

#endif // INTR_MODEL_PRIMITIVE_CACHE_H

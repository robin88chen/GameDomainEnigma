/*********************************************************************
 * \file   IntrGeometryCache.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef INTR_GEOMETRY_CACHE_H
#define INTR_GEOMETRY_CACHE_H

#include "MathLib/IntersectorCache.h"

namespace Enigma::Engine
{
    class IntrGeometryCache : public MathLib::IntersectorCache
    {
    public:
        IntrGeometryCache() : m_cachedIndex{ 0 }, m_requiredResultCount{ 0 } {};

        unsigned int GetElementCachedIndex() const { return m_cachedIndex; };
        void SetElementCachedIndex(unsigned int index) { m_cachedIndex = index; };
        unsigned int GetRequiredResultCount() const { return m_requiredResultCount; };
        void SetRequiredResultCount(unsigned int count) { m_requiredResultCount = count; };
    private:
        unsigned int m_cachedIndex;
        unsigned int m_requiredResultCount;
    };
};

#endif // INTR_GEOMETRY_CACHE_H

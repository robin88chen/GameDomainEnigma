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

namespace Enigma::Geometries
{
    class IntrGeometryCache : public MathLib::IntersectorCache
    {
    public:
        IntrGeometryCache() : m_cachedIndex{ 0 }, m_requiredResultCount{ 0 } {};

        unsigned int getElementCachedIndex() const { return m_cachedIndex; };
        void setElementCachedIndex(unsigned int index) { m_cachedIndex = index; };
        unsigned int getRequiredResultCount() const { return m_requiredResultCount; };
        void setRequiredResultCount(unsigned int count) { m_requiredResultCount = count; };
    private:
        unsigned int m_cachedIndex;
        unsigned int m_requiredResultCount;
    };
};

#endif // INTR_GEOMETRY_CACHE_H

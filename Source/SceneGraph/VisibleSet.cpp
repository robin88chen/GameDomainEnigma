#include "VisibleSet.h"

using namespace Enigma::SceneGraph;

VisibleSet::VisibleSet()
{
}

VisibleSet::~VisibleSet()
{
    m_visibleObjSet.clear();
}

void VisibleSet::Clear()
{
    m_visibleObjSet.clear();
}

size_t VisibleSet::GetCount() const
{
    return m_visibleObjSet.size();
}

const VisibleSet::SpatialVector& VisibleSet::GetObjectSet() const
{
    return m_visibleObjSet;
}

SpatialPtr VisibleSet::GetVisibleObject(size_t index)
{
    if (index >= m_visibleObjSet.size()) return nullptr;
    return m_visibleObjSet[index];
}

void VisibleSet::Insert(const SpatialPtr& obj)
{
    m_visibleObjSet.emplace_back(obj);
}

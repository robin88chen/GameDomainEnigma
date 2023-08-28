#include "Intersector.h"
#include <cassert>

using namespace Enigma::MathLib;

Intersector::Intersector()
{
    m_intersectionType = IntersectionType::EMPTY;
}

Intersector::~Intersector()
{
}

Intersector::Result Intersector::Test(IntersectorCache* /*last_result*/)
{
    // stub for derived class
    assert(false);
    return { false, nullptr };
}

Intersector::Result Intersector::Find(IntersectorCache* last_result)
{
    return Test(last_result);
}

Intersector::IntersectionType Intersector::GetIntersectionType() const
{
    return m_intersectionType;
}


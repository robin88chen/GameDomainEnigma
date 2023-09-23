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

Intersector::Result Intersector::Test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    // stub for derived class
    assert(false);
    return { false, nullptr };
}

Intersector::Result Intersector::Find(std::unique_ptr<IntersectorCache> last_result)
{
    return Test(std::move(last_result));
}

Intersector::IntersectionType Intersector::GetIntersectionType() const
{
    return m_intersectionType;
}


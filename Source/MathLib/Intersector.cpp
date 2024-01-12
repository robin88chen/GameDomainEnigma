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

Intersector::Result Intersector::test(std::unique_ptr<IntersectorCache> /*last_result*/)
{
    // stub for derived class
    assert(false);
    return { false, nullptr };
}

Intersector::Result Intersector::find(std::unique_ptr<IntersectorCache> last_result)
{
    return test(std::move(last_result));
}

Intersector::IntersectionType Intersector::getIntersectionType() const
{
    return m_intersectionType;
}


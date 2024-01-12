#include "Primitive.h"
#include "PrimitiveQueries.h"
#include "Frameworks/QueryDispatcher.h"
#include <cassert>

using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Engine, Primitive);

Primitive::Primitive(const PrimitiveId& id) : m_id(id), m_factoryDesc(Primitive::TYPE_RTTI.getName())
{
    m_mxPrimitiveWorld = MathLib::Matrix4::IDENTITY;
    m_primitiveFlags = 0;
}

Primitive::~Primitive()
{
}

std::shared_ptr<Primitive> Primitive::queryPrimitive(const PrimitiveId& id)
{
    assert(id.rtti().isDerived(Primitive::TYPE_RTTI));
    const auto query = std::make_shared<QueryPrimitive>(id);
    Frameworks::QueryDispatcher::dispatch(query);
    return query->getResult();
}

void Primitive::enumAnimatorListDeep(std::list<std::shared_ptr<Animator>>& resultList)
{
    if (m_animator) resultList.push_back(m_animator);
}

#include "Primitive.h"
#include "PrimitiveQueries.h"
#include "Animators/Animator.h"
#include <cassert>

using namespace Enigma::Primitives;

DEFINE_RTTI_OF_BASE(Primitives, Primitive);

Primitive::Primitive(const PrimitiveId& id) : m_id(id), m_factoryDesc(Primitive::TYPE_RTTI.getName())
{
    m_mxPrimitiveWorld = MathLib::Matrix4::IDENTITY;
    m_primitiveFlags = 0;
}

Primitive::~Primitive()
{
    Animators::Animator::releaseAnimator(m_animatorId);
}

std::shared_ptr<Primitive> Primitive::queryPrimitive(const PrimitiveId& id)
{
    assert(id.rtti().isDerived(Primitive::TYPE_RTTI));
    return std::make_shared<QueryPrimitive>(id)->dispatch();
}

void Primitive::enumAnimatorListDeep(std::list<std::shared_ptr<Animators::Animator>>& resultList)
{
    if (const auto animator = getAnimator()) resultList.push_back(animator);
    //if (m_animator) resultList.push_back(m_animator);
}

std::shared_ptr<Enigma::Animators::Animator> Primitive::getAnimator()
{
    if (m_animatorId.empty()) return nullptr;
    return Animators::Animator::queryAnimator(m_animatorId);
}

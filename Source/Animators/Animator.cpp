#include "Animator.h"
#include "AnimatorQueries.h"
#include "Frameworks/QueryDispatcher.h"

using namespace Enigma::Animators;

DEFINE_RTTI_OF_BASE(Animators, Animator);

Animator::Animator(const AnimatorId& id) : m_id(id), m_isListened(false), m_factoryDesc(Animator::TYPE_RTTI.getName())
{

}

Animator::~Animator()
{

}

std::shared_ptr<Animator> Animator::queryAnimator(const AnimatorId& id)
{
    assert(id.rtti().isDerived(Animator::TYPE_RTTI));
    return std::make_shared<QueryAnimator>(id)->dispatch();
}

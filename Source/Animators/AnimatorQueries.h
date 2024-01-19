/*********************************************************************
 * \file   AnimatorQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATOR_QUERIES_H
#define ANIMATOR_QUERIES_H

#include "Frameworks/Query.h"
#include "AnimatorId.h"

namespace Enigma::Animators
{
    class Animator;

    class QueryAnimator : public Frameworks::Query<std::shared_ptr<Animator>>
    {
    public:
        QueryAnimator(const AnimatorId& id) : m_id(id) {}

        const AnimatorId& id() const { return m_id; }

    protected:
        AnimatorId m_id;
    };
    class QueryAnimatorNextSequenceNumber : public Frameworks::Query<std::uint64_t>
    {
    public:
        QueryAnimatorNextSequenceNumber(const AnimatorId& id) : m_id(id) {}

        const AnimatorId& id() const { return m_id; }

    protected:
        AnimatorId m_id;
    };
}

#endif // ANIMATOR_QUERIES_H

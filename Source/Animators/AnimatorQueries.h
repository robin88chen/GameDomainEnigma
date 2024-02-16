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
#include "GameEngine/GenericDto.h"
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
    class RequestAnimatorCreation : public Frameworks::Query<std::shared_ptr<Animator>>
    {
    public:
        enum class PersistenceLevel { None, Repository, Store };
    public:
        RequestAnimatorCreation(const AnimatorId& id, const Frameworks::Rtti& rtti, PersistenceLevel persistence_level) : m_id(id), m_rtti(rtti.getName()), m_persistenceLevel(persistence_level) {}
        const AnimatorId& id() { return m_id; }
        const Frameworks::Rtti& rtti() { return Frameworks::Rtti::fromName(m_rtti); }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    private:
        AnimatorId m_id;
        std::string m_rtti;
        PersistenceLevel m_persistenceLevel;
    };
    class RequestAnimatorConstitution : public Frameworks::Query<std::shared_ptr<Animator>>
    {
    public:
        enum class PersistenceLevel { None, Repository, Store };
    public:
        RequestAnimatorConstitution(const AnimatorId& id, const Engine::GenericDto& dto, PersistenceLevel persistence_level) : m_id(id), m_dto(dto), m_persistenceLevel(persistence_level) {}
        const AnimatorId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    private:
        AnimatorId m_id;
        Engine::GenericDto m_dto;
        PersistenceLevel m_persistenceLevel;
    };
}

#endif // ANIMATOR_QUERIES_H

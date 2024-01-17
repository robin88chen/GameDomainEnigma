/*********************************************************************
 * \file   AnimatorCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATOR_COMMANDS_H
#define ANIMATOR_COMMANDS_H

#include "Frameworks/Command.h"
#include "AnimatorFactory.h"
#include <string>

namespace Enigma::Engine
{
    class CreateAnimator : public Frameworks::ICommand
    {
    public:
        CreateAnimator(const AnimatorId& id, const Frameworks::Rtti& rtti) : m_id(id), m_rtti(rtti.getName()) {}
        const AnimatorId& id() { return m_id; }
        const Frameworks::Rtti& rtti() { return Frameworks::Rtti::fromName(m_rtti); }

    private:
        AnimatorId m_id;
        std::string m_rtti;
    };
    class ConstituteAnimator : public Frameworks::ICommand
    {
    public:
        ConstituteAnimator(const AnimatorId& id, const GenericDto& dto) : m_id(id), m_dto(dto) {}
        const AnimatorId& id() { return m_id; }
        const GenericDto& dto() { return m_dto; }

    private:
        AnimatorId m_id;
        GenericDto m_dto;
    };
    class PutAnimator : public Frameworks::ICommand
    {
    public:
        PutAnimator(const AnimatorId& id, const std::shared_ptr<Animator>& animator) : m_id(id), m_animator(animator) {}

        const AnimatorId& id() { return m_id; }
        const std::shared_ptr<Animator>& animator() { return m_animator; }

    protected:
        AnimatorId m_id;
        std::shared_ptr<Animator> m_animator;
    };
    class RemoveAnimator : public Frameworks::ICommand
    {
    public:
        RemoveAnimator(const AnimatorId& id) : m_id(id) {}

        const AnimatorId& id() { return m_id; }

    protected:
        AnimatorId m_id;
    };

    class RegisterAnimatorFactory : public Frameworks::ICommand
    {
    public:
        RegisterAnimatorFactory(const std::string& rtti, const AnimatorCreator& creator, const AnimatorConstitutor& constitutor)
            : m_rtti(rtti), m_creator(creator), m_constitutor(constitutor) {}

        const std::string& rttiName() const { return m_rtti; }
        const AnimatorCreator& creator() { return m_creator; }
        const AnimatorConstitutor& constitutor() { return m_constitutor; }

    private:
        std::string m_rtti;
        AnimatorCreator m_creator;
        AnimatorConstitutor m_constitutor;
    };
    class UnregisterAnimatorFactory : public Frameworks::ICommand
    {
    public:
        UnregisterAnimatorFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& rttiName() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
}


#endif // ANIMATOR_COMMANDS_H

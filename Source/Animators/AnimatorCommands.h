﻿/*********************************************************************
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

namespace Enigma::Animators
{
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
    class AddListeningAnimator : public Frameworks::ICommand
    {
    public:
        AddListeningAnimator(const AnimatorId& id) : m_id(id) {}
        const AnimatorId& id() { return m_id; }

    private:
        AnimatorId m_id;
    };
    class RemoveListeningAnimator : public Frameworks::ICommand
    {
    public:
        RemoveListeningAnimator(const AnimatorId& id) : m_id(id) {}
        const AnimatorId& id() { return m_id; }

    private:
        AnimatorId m_id;
    };
}


#endif // ANIMATOR_COMMANDS_H

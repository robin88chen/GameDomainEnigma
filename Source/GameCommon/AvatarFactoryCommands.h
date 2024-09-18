/*********************************************************************
 * \file   AvatarFactoryCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef AVATAR_FACTORY_COMMANDS_H
#define AVATAR_FACTORY_COMMANDS_H

#include "Frameworks/Command.h"
#include "AvatarRecipeFactoryDelegate.h"
#include <string>

namespace Enigma::GameCommon
{
    class RegisterAvatarRecipeFactory : public Frameworks::ICommand
    {
    public:
        RegisterAvatarRecipeFactory(const std::string& rtti, const RecipeCreator& creator) : m_rtti(rtti), m_creator(creator) {}

        [[nodiscard]] const std::string& rtti() const { return m_rtti; }
        [[nodiscard]] RecipeCreator creator() const { return m_creator; }

    protected:
        std::string m_rtti;
        RecipeCreator m_creator;
    };
    class UnregisterAvatarRecipeFactory : public Frameworks::ICommand
    {
    public:
        UnregisterAvatarRecipeFactory(const std::string& rtti) : m_rtti(rtti) {}

        [[nodiscard]] const std::string& rtti() const { return m_rtti; }

    protected:
        std::string m_rtti;
    };
}

#endif // AVATAR_FACTORY_COMMANDS_H

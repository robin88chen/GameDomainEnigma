/*********************************************************************
 * \file   AvatarRecipeQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef AVATAR_RECIPE_QUERIES_H
#define AVATAR_RECIPE_QUERIES_H

#include "Frameworks/Query.h"
#include <string>
#include <memory>

namespace Enigma::GameCommon
{
    class AvatarRecipe;
    class RequestAvatarRecipeCreation : public Frameworks::Query<std::shared_ptr<AvatarRecipe>>
    {
    public:
        RequestAvatarRecipeCreation(const std::string& rtti) : m_rtti(rtti) {}

        [[nodiscard]] const std::string& rtti() const { return m_rtti; }

    protected:
        std::string m_rtti;
    };
}

#endif // AVATAR_RECIPE_QUERIES_H

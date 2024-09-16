/*********************************************************************
 * \file   AvatarRecipeFactoryDelegate.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef AVATAR_RECIPE_FACTORY_DELEGATE_H
#define AVATAR_RECIPE_FACTORY_DELEGATE_H

#include <functional>
#include <memory>

namespace Enigma::GameCommon
{
    class AvatarRecipe;
    using RecipeCreator = std::function<std::shared_ptr<AvatarRecipe>()>;
}

#endif // AVATAR_RECIPE_FACTORY_DELEGATE_H

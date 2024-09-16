/*********************************************************************
 * \file   AvatarRecipeFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef AVATAR_RECIPE_FACTORY_H
#define AVATAR_RECIPE_FACTORY_H

#include "AvatarRecipeFactoryDelegate.h"
#include "Frameworks/SystemService.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/QuerySubscriber.h"

namespace Enigma::GameCommon
{
    class AvatarRecipeFactory : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        AvatarRecipeFactory(Frameworks::ServiceManager* srv_manager);
        AvatarRecipeFactory(const AvatarRecipeFactory&) = delete;
        AvatarRecipeFactory(AvatarRecipeFactory&&) = delete;
        AvatarRecipeFactory& operator=(const AvatarRecipeFactory&) = delete;
        AvatarRecipeFactory& operator=(AvatarRecipeFactory&&) = delete;
        virtual ~AvatarRecipeFactory() override;

        void registerRecipeFactory(const std::string& rtti, RecipeCreator creator);
        void unregisterRecipeFactory(const std::string& rtti);
        std::shared_ptr<AvatarRecipe> createRecipe(const std::string& rtti) const;

    private:
        void registerRecipeFactory(const Frameworks::ICommandPtr& c);
        void unregisterRecipeFactory(const Frameworks::ICommandPtr& c);
        void requestRecipeCreation(const Frameworks::IQueryPtr& q);

    private:
        std::unordered_map<std::string, RecipeCreator> m_recipeCreators;

        Frameworks::CommandSubscriberPtr m_registerRecipeFactory;
        Frameworks::CommandSubscriberPtr m_unregisterRecipeFactory;
        Frameworks::QuerySubscriberPtr m_requestRecipeCreation;
    };
}

#endif // AVATAR_RECIPE_FACTORY_H

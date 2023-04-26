/*********************************************************************
 * \file   AnimatedPawn.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _ANIMATED_PAWN_H
#define _ANIMATED_PAWN_H

#include "SceneGraph/Pawn.h"
#include "AnimationClipMap.h"
namespace Enigma::GameCommon
{
    class AvatarRecipe;

    class AnimatedPawn : public SceneGraph::Pawn
    {
        DECLARE_EN_RTTI;
    public:
        AnimatedPawn(const std::string& name);
        AnimatedPawn(const Engine::GenericDto& o);
        AnimatedPawn(const AnimatedPawn&) = delete;
        AnimatedPawn(AnimatedPawn&&) = delete;
        virtual ~AnimatedPawn() override;
        AnimatedPawn& operator=(const AnimatedPawn&) = delete;
        AnimatedPawn& operator=(AnimatedPawn&&) = delete;

        virtual Engine::GenericDto SerializeDto() override;

        virtual void PlayAnimation(const std::string& name);
        virtual void StopAnimation();

        virtual void AddAvatarRecipe(const std::shared_ptr<AvatarRecipe>& recipe);
        virtual void BakeAvatarRecipes();

    protected:
        AnimationClipMap m_animationClipMap;
        using AvatarRecipeList = std::list<std::shared_ptr<AvatarRecipe>>;
        AvatarRecipeList m_avatarRecipeList;
    };
}

#endif // _ANIMATED_PAWN_H

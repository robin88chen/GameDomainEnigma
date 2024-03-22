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
        AnimatedPawn(const SceneGraph::SpatialId& id);
        AnimatedPawn(const SceneGraph::SpatialId& id, const Engine::GenericDto& o);
        AnimatedPawn(const AnimatedPawn&) = delete;
        AnimatedPawn(AnimatedPawn&&) = delete;
        virtual ~AnimatedPawn() override;
        AnimatedPawn& operator=(const AnimatedPawn&) = delete;
        AnimatedPawn& operator=(AnimatedPawn&&) = delete;

        static std::shared_ptr<AnimatedPawn> create(const SceneGraph::SpatialId& id);
        static std::shared_ptr<AnimatedPawn> constitute(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto);

        virtual Engine::GenericDto serializeDto() override;

        AnimationClipMap& animationClipMap() { return m_animationClipMap; };
        const AnimationClipMap& animationClipMap() const { return m_animationClipMap; };

        virtual void playAnimation(const std::string& name);
        virtual void stopAnimation();

        virtual void addAvatarRecipe(const std::shared_ptr<AvatarRecipe>& recipe);
        virtual void bakeAvatarRecipes();

    protected:
        AnimationClipMap m_animationClipMap;
        using AvatarRecipeList = std::list<std::shared_ptr<AvatarRecipe>>;
        AvatarRecipeList m_avatarRecipeList;
    };
}

#endif // _ANIMATED_PAWN_H

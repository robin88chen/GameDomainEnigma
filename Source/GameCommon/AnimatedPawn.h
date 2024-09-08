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
#include "Frameworks/EventSubscriber.h"

namespace Enigma::GameCommon
{
    class AvatarRecipe;

    class AnimatedPawn : public SceneGraph::Pawn
    {
        DECLARE_EN_RTTI;
    public:
        AnimatedPawn(const SceneGraph::SpatialId& id);
        AnimatedPawn(const AnimatedPawn&) = delete;
        AnimatedPawn(AnimatedPawn&&) = delete;
        virtual ~AnimatedPawn() override;
        AnimatedPawn& operator=(const AnimatedPawn&) = delete;
        AnimatedPawn& operator=(AnimatedPawn&&) = delete;

        static std::shared_ptr<AnimatedPawn> create(const SceneGraph::SpatialId& id);

        virtual std::shared_ptr<SceneGraph::SpatialAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<SceneGraph::SpatialAssembler>& assembler) override;
        virtual std::shared_ptr<SceneGraph::SpatialDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<SceneGraph::SpatialDisassembler>& disassembler) override;

        void registerHandlers();
        void unregisterHandlers();

        AnimationClipMap& animationClipMap() { return m_animationClipMap; };
        const AnimationClipMap& animationClipMap() const { return m_animationClipMap; };

        virtual void playAnimation(const std::string& name);
        virtual void stopAnimation();

        virtual void addAvatarRecipe(const std::shared_ptr<AvatarRecipe>& recipe);
        virtual void bakeAvatarRecipes();

    protected:
        void onRenderablePrimitiveHydrated(const Frameworks::IEventPtr& e);

    protected:
        AnimationClipMap m_animationClipMap;
        using AvatarRecipeList = std::list<std::shared_ptr<AvatarRecipe>>;
        AvatarRecipeList m_avatarRecipeList;

        Frameworks::EventSubscriberPtr m_onRenderablePrimitiveHydrated;
    };
}

#endif // _ANIMATED_PAWN_H

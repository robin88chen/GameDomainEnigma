/*****************************************************************
 * \file   AnimatedPawnAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef ANIMATED_PAWN_ASSEMBLER_H
#define ANIMATED_PAWN_ASSEMBLER_H

#include "SceneGraph/PawnAssembler.h"
#include "AnimatedPawn.h"

namespace Enigma::GameCommon
{
    class AnimationClipMapAssembler;
    class AnimationClipMapDisassembler;
    class AvatarRecipeAssembler;
    class AvatarRecipeDisassembler;

    class AnimatedPawnAssembler : public SceneGraph::PawnAssembler
    {
    public:
        AnimatedPawnAssembler(const SceneGraph::SpatialId& id);

        virtual void factory(const Engine::FactoryDesc& factory) override;
        virtual void persist(const std::string& filename, const std::string& path_id) override;

        void animationClipMap(const std::shared_ptr<AnimationClipMapAssembler>& clip);
        void animationClipMap(const AnimationClipMap& clip);
        void byPrefab(const std::string& prefab_name);
        void addAvatarRecipe(const std::shared_ptr<AvatarRecipe>& recipe);
        void addAvatarRecipe(const std::shared_ptr<AvatarRecipeAssembler>& recipe);

        Engine::GenericDto assemble() const override;

    private:
        std::shared_ptr<AnimationClipMapAssembler> m_clip;
        std::optional<std::string> m_prefab;
        std::vector<std::shared_ptr<AvatarRecipeAssembler>> m_avatarRecipes;
    };
    class AnimatedPawnDisassembler : public SceneGraph::PawnDisassembler
    {
    public:
        AnimatedPawnDisassembler();

        void disassemble(const Engine::GenericDto& dto) override;

        [[nodiscard]] const AnimationClipMap& animationClipMap() const { return m_animationClipMap; }
        [[nodiscard]] const std::vector<std::shared_ptr<AvatarRecipe>>& avatarRecipes() const { return m_avatarRecipes; }

    protected:
        AnimationClipMap m_animationClipMap;
        std::vector<std::shared_ptr<AvatarRecipe>> m_avatarRecipes;
    };
}

#endif // ANIMATED_PAWN_ASSEMBLER_H

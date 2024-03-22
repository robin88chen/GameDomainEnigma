/*****************************************************************
 * \file   AnimatedPawnAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef ANIMATED_PAWN_ASSEMBLER_H
#define ANIMATED_PAWN_ASSEMBLER_H

#include "SceneGraph/SceneGraphAssemblers.h"
#include "AnimationClipMapAssembler.h"
#include "AnimatedPawnDto.h"
#include "AnimatedPawn.h"

namespace Enigma::GameCommon
{
    class AnimatedPawnAssembler
    {
    public:
        AnimatedPawnAssembler(const SceneGraph::SpatialId& id);

        const SceneGraph::SpatialId& id() const { return m_id; }

        SceneGraph::PawnAssembler& pawn();

        AnimatedPawnAssembler& factory(const Engine::FactoryDesc& factory);
        AnimatedPawnAssembler& animationClipMap(const AnimationClipMapAssembler& clip);
        AnimatedPawnAssembler& asNative(const std::string& file_at_path);
        AnimatedPawnAssembler& byPrefab(const std::string& prefab_name);

        Engine::GenericDto toGenericDto();
        AnimatedPawnDto toAnimatedPawnDto();
        std::shared_ptr<AnimatedPawn> constitute(SceneGraph::PersistenceLevel persistence_level);

    private:
        SceneGraph::SpatialId m_id;
        SceneGraph::PawnAssembler m_pawnAssembler;
        AnimatedPawnDto m_dto;
    };
}

#endif // ANIMATED_PAWN_ASSEMBLER_H

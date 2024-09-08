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
#include "AnimationClipMapAssembler.h"
#include "AnimatedPawnDto.h"
#include "AnimatedPawn.h"

namespace Enigma::GameCommon
{
    class AnimatedPawnAssembler : public SceneGraph::PawnAssembler
    {
    public:
        AnimatedPawnAssembler(const SceneGraph::SpatialId& id);

        virtual void factory(const Engine::FactoryDesc& factory) override;
        virtual void persist(const std::string& filename, const std::string& path_id) override;

        void animationClipMap(const std::shared_ptr<AnimationClipMapAssembler>& clip);
        void animationClipMap(const AnimationClipMap& clip);
        void byPrefab(const std::string& prefab_name);
        void avatarRecipeDtos(const Engine::GenericDtoCollection& dtos) { m_avatarRecipeDtos = dtos; }

        Engine::GenericDto assemble() const override;

    private:
        std::shared_ptr<AnimationClipMapAssembler> m_clip;
        std::optional<std::string> m_prefab;
        Engine::GenericDtoCollection m_avatarRecipeDtos;
    };
    class AnimatedPawnDisassembler : public SceneGraph::PawnDisassembler
    {
    public:
        AnimatedPawnDisassembler();

        void disassemble(const Engine::GenericDto& dto) override;

        [[nodiscard]] const std::optional<Engine::GenericDto>& animationClipMapDto() const { return m_animationClipMapDto; }
        [[nodiscard]] const Engine::GenericDtoCollection& avatarRecipeDtos() const { return m_avatarRecipeDtos; }

    protected:
        std::optional<Engine::GenericDto> m_animationClipMapDto;
        Engine::GenericDtoCollection m_avatarRecipeDtos;
    };
}

#endif // ANIMATED_PAWN_ASSEMBLER_H

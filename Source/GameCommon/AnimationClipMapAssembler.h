/*****************************************************************
 * \file   AnimationClipMapAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef ANIMATION_CLIP_MAP_ASSEMBLER_H
#define ANIMATION_CLIP_MAP_ASSEMBLER_H

#include "Renderables/AnimationClip.h"
#include "AnimationClipMapDto.h"
#include <string>

namespace Enigma::GameCommon
{
    class AnimationClipMapAssembler
    {
    public:
        AnimationClipMapAssembler() = default;

        AnimationClipMapAssembler& addClip(const std::string& name, const Renderables::AnimationClip& clip);

        Engine::GenericDto toGenericDto() const;

    private:
        AnimationClipMapDto m_dto;
    };
}

#endif // ANIMATION_CLIP_MAP_ASSEMBLER_H

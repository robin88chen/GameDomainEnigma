/*********************************************************************
 * \file   AnimationAssetAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_ASSEMBLER_H
#define ANIMATION_ASSET_ASSEMBLER_H

#include "AnimationAssetId.h"
#include "GameEngine/FactoryDesc.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::Animators
{
    class AnimationAssetAssembler
    {
    public:
        AnimationAssetAssembler(const AnimationAssetId& id);
        virtual ~AnimationAssetAssembler() = default;
        AnimationAssetAssembler(const AnimationAssetAssembler&) = default;
        AnimationAssetAssembler(AnimationAssetAssembler&&) = default;
        AnimationAssetAssembler& operator=(const AnimationAssetAssembler&) = default;
        AnimationAssetAssembler& operator=(AnimationAssetAssembler&&) = default;

        void factoryDesc(const Engine::FactoryDesc& factoryDesc) { m_factoryDesc = factoryDesc; }

        virtual Engine::GenericDto assemble() const = 0;

    protected:
        AnimationAssetId m_id;
        Engine::FactoryDesc m_factoryDesc;
    };

    class AnimationAssetDisassembler
    {
    public:
        AnimationAssetDisassembler();
        virtual ~AnimationAssetDisassembler() = default;
        AnimationAssetDisassembler(const AnimationAssetDisassembler&) = default;
        AnimationAssetDisassembler(AnimationAssetDisassembler&&) = default;
        AnimationAssetDisassembler& operator=(const AnimationAssetDisassembler&) = default;
        AnimationAssetDisassembler& operator=(AnimationAssetDisassembler&&) = default;

        [[nodiscard]] const AnimationAssetId& id() const { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }

        virtual void disassemble(const Engine::GenericDto& dto) = 0;

    protected:
        AnimationAssetId m_id;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // ANIMATION_ASSET_ASSEMBLER_H

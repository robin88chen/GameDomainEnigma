/*********************************************************************
 * \file   AnimationAsset.h
 * \brief  animation asset base class, entity, 因為資料龐大, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_ASSET_H
#define _ANIMATION_ASSET_H

#include "Frameworks/Rtti.h"
#include "GameEngine/FactoryDesc.h"
#include "AnimationAssetId.h"
#include "GameEngine/GenericDto.h"
#include <string>

namespace Enigma::Animators
{
    class AnimationAssetAssembler;
    class AnimationAssetDisassembler;

    class AnimationAsset
    {
        DECLARE_EN_RTTI_OF_BASE
    public:
        AnimationAsset(const AnimationAssetId& id);
        AnimationAsset(const AnimationAsset&) = delete;
        AnimationAsset(AnimationAsset&&) = delete;
        virtual ~AnimationAsset();
        AnimationAsset& operator=(const AnimationAsset&) = delete;
        AnimationAsset& operator=(AnimationAsset&&) = delete;

        const AnimationAssetId& id() { return m_id; }

        virtual std::shared_ptr<AnimationAssetAssembler> assembler() const = 0;
        virtual void assemble(const std::shared_ptr<AnimationAssetAssembler>& assembler) const = 0; ///< that's double dispatch
        virtual std::shared_ptr<AnimationAssetDisassembler> disassembler() const = 0;
        virtual void disassemble(const std::shared_ptr<AnimationAssetDisassembler>& disassembler) = 0; ///< that's double dispatch

        static std::shared_ptr<AnimationAsset> queryAnimationAsset(const AnimationAssetId& id);

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        virtual float getAnimationLengthInSecond() = 0;

    protected:
        AnimationAssetId m_id;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // _ANIMATION_ASSET_H

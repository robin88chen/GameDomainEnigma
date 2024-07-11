/*********************************************************************
 * \file   AnimationAssetCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_COMMANDS_H
#define ANIMATION_ASSET_COMMANDS_H

#include "Frameworks/Command.h"
#include "AnimationAssetFactory.h"
#include <string>

namespace Enigma::Animators
{
    class PutAnimationAsset : public Frameworks::ICommand
    {
    public:
        PutAnimationAsset(const AnimationAssetId& id, const std::shared_ptr<AnimationAsset>& animation) : m_id(id), m_animation(animation) {}

        const AnimationAssetId& id() { return m_id; }
        const std::shared_ptr<AnimationAsset>& animation() { return m_animation; }

    protected:
        AnimationAssetId m_id;
        std::shared_ptr<AnimationAsset> m_animation;
    };
    class RemoveAnimationAsset : public Frameworks::ICommand
    {
    public:
        RemoveAnimationAsset(const AnimationAssetId& id) : m_id(id) {}

        const AnimationAssetId& id() { return m_id; }

    protected:
        AnimationAssetId m_id;
    };
    class ReleaseAnimationAsset : public Frameworks::ICommand
    {
    public:
        ReleaseAnimationAsset(const AnimationAssetId& id) : m_id(id) {}

        const AnimationAssetId& id() { return m_id; }

    protected:
        AnimationAssetId m_id;
    };
    class RegisterAnimationAssetFactory : public Frameworks::ICommand
    {
    public:
        RegisterAnimationAssetFactory(const std::string& rtti, const AnimationAssetCreator& creator, const AnimationAssetConstitutor& constitutor)
            : m_rtti(rtti), m_creator(creator), m_constitutor(constitutor) {}

        const std::string& rttiName() const { return m_rtti; }
        const AnimationAssetCreator& creator() { return m_creator; }
        const AnimationAssetConstitutor& constitutor() { return m_constitutor; }

    private:
        std::string m_rtti;
        AnimationAssetCreator m_creator;
        AnimationAssetConstitutor m_constitutor;
    };
    class UnregisterAnimationAssetFactory : public Frameworks::ICommand
    {
    public:
        UnregisterAnimationAssetFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& rttiName() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
}


#endif // ANIMATION_ASSET_COMMANDS_H

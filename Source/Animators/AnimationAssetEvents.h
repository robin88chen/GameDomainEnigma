/*********************************************************************
 * \file   AnimationAssetEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_ASSET_EVENTS_H
#define _ANIMATION_ASSET_EVENTS_H

#include "Frameworks/Event.h"
#include "Frameworks/ruid.h"
#include <string>
#include <memory>

namespace Enigma::Animators
{
    class AnimationAsset;

    class AnimationAssetBuilt : public Frameworks::IEvent
    {
    public:
        AnimationAssetBuilt(const std::string& name, const std::shared_ptr<AnimationAsset>& ani) :
            m_name(name), m_animation(ani) {};
        const std::string& getName() { return m_name; }
        const std::shared_ptr<AnimationAsset>& GetAnimationAsset() { return m_animation; }
    private:
        std::string m_name;
        std::shared_ptr<AnimationAsset> m_animation;
    };
    class BuildAnimationAssetFailed : public Frameworks::IEvent
    {
    public:
        BuildAnimationAssetFailed(const std::string& name, std::error_code er) :
            m_name(name), m_error(er) {};
        const std::string& getName() { return m_name; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        std::string m_name;
        std::error_code m_error;
    };
    class FactoryAnimationAssetCreated : public Frameworks::IEvent
    {
    public:
        FactoryAnimationAssetCreated(const Frameworks::Ruid& ruid, const std::shared_ptr<AnimationAsset>& ani)
            : m_ruid(ruid), m_animation(ani) {};

        const Frameworks::Ruid& GetConstructingRuid() const { return m_ruid; }
        const std::shared_ptr<AnimationAsset>& GetAnimationAsset() { return m_animation; }

    protected:
        Frameworks::Ruid m_ruid;
        std::shared_ptr<AnimationAsset> m_animation;
    };
}

#endif // _ANIMATION_ASSET_EVENTS_H

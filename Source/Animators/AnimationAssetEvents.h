/*********************************************************************
 * \file   AnimationAssetEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_EVENTS_H
#define ANIMATION_ASSET_EVENTS_H

#include "Frameworks/Event.h"
#include "AnimationAsset.h"
#include <system_error>

namespace Enigma::Animators
{
    class AnimationAssetCreated : public Frameworks::IEvent
    {
    public:
        AnimationAssetCreated(const AnimationAssetId& id, const std::shared_ptr<AnimationAsset>& animation) :
            m_id(id), m_animation(animation) {};
        const AnimationAssetId& id() { return m_id; }
        const std::shared_ptr<AnimationAsset>& animation() { return m_animation; }
    private:
        AnimationAssetId m_id;
        std::shared_ptr<AnimationAsset> m_animation;
    };
    class CreateAnimationAssetFailed : public Frameworks::IEvent
    {
    public:
        CreateAnimationAssetFailed(const AnimationAssetId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const AnimationAssetId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimationAssetId m_id;
        std::error_code m_error;
    };
    class AnimationAssetConstituted : public Frameworks::IEvent
    {
    public:
        AnimationAssetConstituted(const AnimationAssetId& id, const std::shared_ptr<AnimationAsset>& animation, bool is_persisted) :
            m_id(id), m_animation(animation), m_is_persisted(is_persisted) {};
        const AnimationAssetId& id() { return m_id; }
        const std::shared_ptr<AnimationAsset>& animation() { return m_animation; }
        bool isPersisted() const { return m_is_persisted; }
    private:
        AnimationAssetId m_id;
        std::shared_ptr<AnimationAsset> m_animation;
        bool m_is_persisted;
    };
    class ConstituteAnimationAssetFailed : public Frameworks::IEvent
    {
    public:
        ConstituteAnimationAssetFailed(const AnimationAssetId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const AnimationAssetId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimationAssetId m_id;
        std::error_code m_error;
    };
    class AnimationAssetRemoved : public Frameworks::IEvent
    {
    public:
        AnimationAssetRemoved(const AnimationAssetId& id) : m_id(id) {};
        const AnimationAssetId& id() { return m_id; }
    private:
        AnimationAssetId m_id;
    };
    class RemoveAnimationAssetFailed : public Frameworks::IEvent
    {
    public:
        RemoveAnimationAssetFailed(const AnimationAssetId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const AnimationAssetId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimationAssetId m_id;
        std::error_code m_error;
    };
    class AnimationAssetPut : public Frameworks::IEvent
    {
    public:
        AnimationAssetPut(const AnimationAssetId& id) : m_id(id) {};
        const AnimationAssetId& id() { return m_id; }
    private:
        AnimationAssetId m_id;
    };
    class PutAnimationAssetFailed : public Frameworks::IEvent
    {
    public:
        PutAnimationAssetFailed(const AnimationAssetId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const AnimationAssetId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimationAssetId m_id;
        std::error_code m_error;
    };
    class AnimationAssetReleased : public Frameworks::IEvent
    {
    public:
        AnimationAssetReleased(const AnimationAssetId& id) : m_id(id) {}
        const AnimationAssetId& id() { return m_id; }
    private:
        AnimationAssetId m_id;
    };
    class ReleaseAnimationAssetFailed : public Frameworks::IEvent
    {
    public:
        ReleaseAnimationAssetFailed(const AnimationAssetId& id, std::error_code er) : m_id(id), m_error(er) {};
        const AnimationAssetId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimationAssetId m_id;
        std::error_code m_error;
    };
}

#endif // ANIMATION_ASSET_EVENTS_H

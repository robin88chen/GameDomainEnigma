/*********************************************************************
 * \file   AnimationClipMap.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _ANIMATION_CLIP_MAP_H
#define _ANIMATION_CLIP_MAP_H

#include <string>
#include <unordered_map>
#include <optional>
#include "Renderables/AnimationClip.h"
#include "Frameworks/optional_ref.hpp"
#include "Frameworks/Rtti.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::GameCommon
{
    class AnimationClipMapDto;

    class AnimationClipMap
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        class AnimClip
        {
        public:
            AnimClip() {};
            AnimClip(const std::string& name, const Renderables::AnimationClip& clip)
            {
                m_actionName = name; m_animClip = clip;
            };
            // copy-er, move-er, destructor 會自動建立
            const std::string& getName() const { return m_actionName; };
            void changeClip(const Renderables::AnimationClip& clip) { m_animClip = clip; };
            const Renderables::AnimationClip& clip() const { return m_animClip; };

        private:
            std::string m_actionName;
            Renderables::AnimationClip m_animClip;
        };
        typedef std::unordered_map<std::string, AnimClip> ClipMap;
    public:
        AnimationClipMap() = default;
        AnimationClipMap(const Engine::GenericDto& o);
        AnimationClipMap(const AnimationClipMap&) = default;
        AnimationClipMap(AnimationClipMap&&) = default;
        virtual ~AnimationClipMap() = default;
        AnimationClipMap& operator=(const AnimationClipMap&) = default;
        AnimationClipMap& operator=(AnimationClipMap&&) = default;

        Engine::GenericDto serializeDto() const;

        stdext::optional_ref<AnimClip> findAnimationClip(const std::string& name);
        std::optional<AnimClip> findAnimationClip(const std::string& name) const;
        unsigned size() const { return static_cast<unsigned int>(m_animClips.size()); };
        const ClipMap& animationClipMap() const { return m_animClips; };

        void insertClip(const AnimClip& anim_clip);
        void removeClip(const std::string& name);

    protected:
        ClipMap m_animClips;
    };
}

#endif // _ANIMATION_CLIP_MAP_H

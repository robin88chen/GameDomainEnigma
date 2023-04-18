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
#include "Animators/AnimationClip.h"

namespace Enigma::GameCommon
{
    class AnimationClipMap
    {
    public:
        struct AnimClip
        {
            std::string m_actionName;
            Animators::AnimationClip m_animClip;
            AnimClip() {};
            AnimClip(const std::string& name, const Animators::AnimationClip& clip)
            {
                m_actionName = name; m_animClip = clip;
            };
            // copy-er, move-er, destructor 會自動建立
        };
        typedef std::unordered_map<std::string, AnimClip> ClipMap;
    public:
        AnimationClipMap() = default;
        ~AnimationClipMap() = default;

        std::optional<AnimClip> FindAnimationClip(const std::string& name);
        unsigned GetSize() const { return static_cast<unsigned int>(m_animClips.size()); };
        const ClipMap& GetAnimationClipMap() { return m_animClips; };

        void InsertClip(const AnimClip& anim_clip);
        void RemoveClip(const std::string& name);

    protected:
        ClipMap m_animClips;
    };
}

#endif // _ANIMATION_CLIP_MAP_H

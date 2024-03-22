/*********************************************************************
 * \file   AnimationClipMapEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _ANIMATION_CLIP_MAP_EVENTS_H
#define _ANIMATION_CLIP_MAP_EVENTS_H

#include "Frameworks/Event.h"
#include "AnimationClipMap.h"

namespace Enigma::GameCommon
{
    class AnimationClipMapChanged : public Frameworks::IEvent
    {
    public:
        AnimationClipMapChanged(const AnimationClipMap::ClipMap& clip_map) : m_clipMap(clip_map) {};

        const AnimationClipMap::ClipMap& clipMap() const { return m_clipMap; };
    private:
        AnimationClipMap::ClipMap m_clipMap;
    };
}

#endif // _ANIMATION_CLIP_MAP_EVENTS_H

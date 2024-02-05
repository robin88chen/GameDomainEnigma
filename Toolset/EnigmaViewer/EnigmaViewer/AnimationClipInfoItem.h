/*********************************************************************
 * \file   AnimationClipInfoItem.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _ANIMATION_CLIP_INFO_ITEM_H
#define _ANIMATION_CLIP_INFO_ITEM_H

#include <string>
#include "nana/gui/widgets/listbox.hpp"
#include "Renderables/AnimationClip.h"

namespace EnigmaViewer
{
    struct AnimClipInfoItem
    {
        std::string m_name;
        Enigma::Renderables::AnimationClip m_clip;
        AnimClipInfoItem(const std::string& name) : m_name{ name } {}
    };

    nana::listbox::iresolver& operator>>(nana::listbox::iresolver& ires, AnimClipInfoItem& item)
    {
        float start, loop;
        unsigned int div;
        std::string wrap;
        Enigma::Renderables::AnimationClip::WarpMode mode = Enigma::Renderables::AnimationClip::WarpMode::Loop;

        ires >> item.m_name >> start >> loop >> div >> wrap;
        if (wrap == "Clamp") mode = Enigma::Renderables::AnimationClip::WarpMode::Clamp;
        item.m_clip = Enigma::Renderables::AnimationClip{ start, loop, mode, div };
        return ires;
    }
    //Overload the operator<< for oresolver to resolve the person type
    nana::listbox::oresolver& operator<<(nana::listbox::oresolver& ores, const AnimClipInfoItem& item)
    {
        std::string warp = "Loop";
        if (item.m_clip.warpMode() == Enigma::Renderables::AnimationClip::WarpMode::Clamp)
        {
            warp = "Clamp";
        }
        return ores << item.m_name << item.m_clip.startOffset() << item.m_clip.loopTime()
            << item.m_clip.divideIndex() << warp;
    }
}


#endif // _ANIMATION_CLIP_INFO_ITEM_H

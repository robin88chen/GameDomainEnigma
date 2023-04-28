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
#include "Animators/AnimationClip.h"

namespace EnigmaViewer
{
    struct AnimClipInfoItem
    {
        std::string m_name;
        Enigma::Animators::AnimationClip m_clip;
        AnimClipInfoItem(const std::string& name) : m_name{ name } {}
    };

    nana::listbox::iresolver& operator>>(nana::listbox::iresolver& ires, AnimClipInfoItem& item)
    {
        float start, loop;
        unsigned int div;
        std::string wrap;
        Enigma::Animators::AnimationClip::WarpMode mode = Enigma::Animators::AnimationClip::WarpMode::Loop;

        ires >> item.m_name >> start >> loop >> div >> wrap;
        if (wrap == "Clamp") mode = Enigma::Animators::AnimationClip::WarpMode::Clamp;
        item.m_clip = Enigma::Animators::AnimationClip{ start, loop, mode, div };
        return ires;
    }
    //Overload the operator<< for oresolver to resolve the person type
    nana::listbox::oresolver& operator<<(nana::listbox::oresolver& ores, const AnimClipInfoItem& item)
    {
        std::string warp = "Loop";
        if (item.m_clip.GetWarpMode() == Enigma::Animators::AnimationClip::WarpMode::Clamp)
        {
            warp = "Clamp";
        }
        return ores << item.m_name << item.m_clip.GetStartOffset() << item.m_clip.GetLoopTime()
            << item.m_clip.GetDivideIndex() << warp;
    }
}


#endif // _ANIMATION_CLIP_INFO_ITEM_H

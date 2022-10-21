/*********************************************************************
 * \file   SchemeColorDef.h
 * \brief  color define for UI scheme
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _VIEWER_COLOR_DEF_H
#define _VIEWER_COLOR_DEF_H

#include "nana/basic_types.hpp"
#include "nana/gui/detail/widget_geometrics.hpp"
namespace LevelEditor
{
    class UISchemeColors
    {
    public:
        static void ApplySchemaColors(nana::widget_geometrics& scheme);
        static nana::color BACKGROUND;
        static nana::color FOREGROUND;
        static nana::color SELECT_BG;
        static nana::color HIGHLIGHT_BG;
    };
}
#endif // !_VIEWER_COLOR_DEF_H

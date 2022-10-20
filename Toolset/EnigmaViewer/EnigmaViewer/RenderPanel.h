/*********************************************************************
 * \file   RenderPanel.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _RENDER_PANEL_H
#define _RENDER_PANEL_H

#include "nana/gui.hpp"
#include "nana/gui/widgets/form.hpp"
#include "nana/gui/widgets/button.hpp"

namespace EnigmaViewer
{
    class RenderPanel : public nana::nested_form
    {
    public:
        RenderPanel(nana::window wd);
        virtual ~RenderPanel();

        void OnMouseDown(const nana::arg_mouse& arg);
        void OnMouseUp(const nana::arg_mouse& arg);
        void OnMouseMove(const nana::arg_mouse& arg);
        void OnMouseWheel(const nana::arg_wheel& arg);

        void OnResized(const nana::arg_resized& arg);
    private:
        bool m_isDraging;
        nana::point m_lastPos;
    };
}

#endif // _RENDER_PANEL_H

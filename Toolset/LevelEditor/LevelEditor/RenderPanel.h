/*********************************************************************
 * \file   RenderPanel.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RENDER_PANEL_H
#define RENDER_PANEL_H

#include "nana/gui.hpp"
#include "nana/gui/widgets/form.hpp"
#include "nana/gui/widgets/button.hpp"

namespace LevelEditor
{
    class RenderPanel : public nana::nested_form
    {
    public:
        RenderPanel(nana::window wd);
        virtual ~RenderPanel();

        std::tuple<float, float> ClientXYToClippingXY(int x, int y);

        void OnMouseDown(const nana::arg_mouse& arg);
        void OnMouseUp(const nana::arg_mouse& arg);
        void OnMouseMove(const nana::arg_mouse& arg);
        void OnMouseWheel(const nana::arg_wheel& arg);

        void OnResized(const nana::arg_resized& arg);
    };
}


#endif // RENDER_PANEL_H

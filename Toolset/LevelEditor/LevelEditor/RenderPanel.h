/*********************************************************************
 * \file   RenderPanel.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef RENDER_PANEL_H
#define RENDER_PANEL_H

#include <Frameworks/EventSubscriber.h>

#include "Frameworks/Event.h"
#include "InputHandlers/InputHandlerService.h"
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

        void SubscribeHandlers();
        void UnsubscribeHandlers();

        void InitInputHandler(const std::shared_ptr<Enigma::InputHandlers::InputHandlerService>& input_handler) { m_inputHandler = input_handler; }
    private:
        void OnRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

    private:
        Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
        std::weak_ptr<Enigma::InputHandlers::InputHandlerService> m_inputHandler;
    };
}


#endif // RENDER_PANEL_H

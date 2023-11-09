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

        std::tuple<float, float> clientXYToClippingXY(int x, int y);

        void onMouseDown(const nana::arg_mouse& arg);
        void onMouseUp(const nana::arg_mouse& arg);
        void onMouseMove(const nana::arg_mouse& arg);
        void onMouseWheel(const nana::arg_wheel& arg);

        void onResized(const nana::arg_resized& arg);

        void subscribeHandlers();
        void unsubscribeHandlers();

        void initInputHandler(const std::shared_ptr<Enigma::InputHandlers::InputHandlerService>& input_handler) { m_inputHandler = input_handler; }
    private:
        void onRenderTargetCreated(const Enigma::Frameworks::IEventPtr& e);

    private:
        Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
        std::weak_ptr<Enigma::InputHandlers::InputHandlerService> m_inputHandler;
    };
}


#endif // RENDER_PANEL_H

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
#include "Frameworks/EventSubscriber.h"
#include "InputHandlers/InputHandlerService.h"

namespace EnigmaViewer
{
    class RenderPanel : public nana::nested_form
    {
    public:
        RenderPanel(nana::window wd);
        virtual ~RenderPanel();

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
        bool m_isDraging;
        nana::point m_lastPos;

        Enigma::Frameworks::EventSubscriberPtr m_onRenderTargetCreated;
        std::weak_ptr<Enigma::InputHandlers::InputHandlerService> m_inputHandler;
    };
}

#endif // _RENDER_PANEL_H

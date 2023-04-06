/*********************************************************************
 * \file   OutputPanel.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _OUTPUT_PANEL_H
#define _OUTPUT_PANEL_H

#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/widgets/textbox.hpp"
#include "nana/gui/place.hpp"
#include "Frameworks/CommandSubscriber.h"

namespace EnigmaViewer
{
    class OutputPanel : public nana::panel<false>
    {
    public:
        OutputPanel(const nana::window& wd);
        virtual ~OutputPanel();

        void Initialize();
        void SubscribeHandlers();
        void UnsubscribeHandlers();

        void AddMessage(const std::string& msg);

    private:
        void DoOutputMessage(const Enigma::Frameworks::ICommandPtr& c);
    private:
        nana::place* m_place;
        nana::textbox* m_outputText;
        Enigma::Frameworks::CommandSubscriberPtr m_doOutputMessage;
    };
}

#endif // _OUTPUT_PANEL_H

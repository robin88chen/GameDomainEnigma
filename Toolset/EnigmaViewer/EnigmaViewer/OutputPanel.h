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

namespace EnigmaViewer
{
    class OutputPanel : public nana::panel<false>
    {
    public:
        OutputPanel(const nana::window& wd);
        virtual ~OutputPanel();

        void Initialize();

        void AddMessage(const std::string& msg);

    private:
        nana::place* m_place;
        nana::textbox* m_outputText;
    };
}

#endif // _OUTPUT_PANEL_H

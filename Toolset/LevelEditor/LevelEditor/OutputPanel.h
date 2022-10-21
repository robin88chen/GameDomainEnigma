/*********************************************************************
 * \file   OutputPanel.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef OUT_PUTPANEL_H
#define OUT_PUTPANEL_H

#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/widgets/textbox.hpp"
#include "nana/gui/place.hpp"

namespace LevelEditor
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


#endif // OUT_PUTPANEL_H

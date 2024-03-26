/*****************************************************************
 * \file   PawnListPanel.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef PAWN_LIST_PANEL_H
#define PAWN_LIST_PANEL_H

#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/listbox.hpp"

namespace EnigmaViewer
{
    class MainForm;

    class PawnListPanel : public nana::panel<false>
    {
    public:
        PawnListPanel(const nana::window& wd);
        virtual ~PawnListPanel();

        void initialize(MainForm* main_form);

    private:
        nana::place* m_place;
        nana::listbox* m_pawnList;
    };
}

#endif // PAWN_LIST_PANEL_H

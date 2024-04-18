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
#include "nana/gui/widgets/menu.hpp"
#include "Frameworks/CommandSubscriber.h"

namespace EnigmaViewer
{
    class MainForm;

    class PawnListPanel : public nana::panel<false>
    {
    public:
        PawnListPanel(const nana::window& wd);
        virtual ~PawnListPanel();

        void initialize(MainForm* main_form);
        void subscribeHandlers();
        void unsubscribeHandlers();

    private:
        void refreshPawnList(const Enigma::Frameworks::ICommandPtr& c);

        void onPawnListMouseDown(const nana::arg_mouse& arg);
        void onLoadPawn(nana::menu::item_proxy& item);
        void onRemovePawn(nana::menu::item_proxy& item);

    private:
        nana::place* m_place;
        nana::listbox* m_pawnList;
        nana::menu* m_popupMenu;

        Enigma::Frameworks::CommandSubscriberPtr m_refreshPawnList;
    };
}

#endif // PAWN_LIST_PANEL_H

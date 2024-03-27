/*****************************************************************
 * \file   ModelListPanel.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2024
 ******************************************************************/
#ifndef MODEL_LIST_PANEL_H
#define MODEL_LIST_PANEL_H

#include "nana/gui/widgets/menu.hpp"
#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/listbox.hpp"
#include "Frameworks/CommandSubscriber.h"

namespace EnigmaViewer
{
    class MainForm;
    class ModelListPanel : public nana::panel<false>
    {
    public:
        ModelListPanel(const nana::window& wd);
        virtual ~ModelListPanel();

        void initialize(MainForm* main_form);
        void subscribeHandlers();
        void unsubscribeHandlers();

    private:
        void refreshModelList(const Enigma::Frameworks::ICommandPtr& c);

        void onModelListMouseDown(const nana::arg_mouse& arg);
        void onLoadModel(nana::menu::item_proxy& item);
    private:
        nana::place* m_place;
        nana::listbox* m_modelList;
        nana::menu* m_popupMenu;

        Enigma::Frameworks::CommandSubscriberPtr m_refreshModelList;
    };
}

#endif // MODEL_LIST_PANEL_H

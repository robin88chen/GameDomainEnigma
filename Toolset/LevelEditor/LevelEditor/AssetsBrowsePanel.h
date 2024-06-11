/*********************************************************************
 * \file   AssetsBrowsePanel.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 *********************************************************************/
#ifndef ASSETS_BROWSE_PANEL_H
#define ASSETS_BROWSE_PANEL_H

#include <nana/gui/place.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/listbox.hpp>

namespace LevelEditor
{
    class MainForm;

    class AssetsBrowsePanel : public nana::panel<false>
    {
    public:
        AssetsBrowsePanel(const nana::window& wd);
        virtual ~AssetsBrowsePanel();

        void initialize(MainForm* main_form);
        void finalize();

    protected:
        MainForm* m_mainForm;
        nana::place* m_place;
        nana::listbox* m_assetsList;
    };
}

#endif // ASSETS_BROWSE_PANEL_H

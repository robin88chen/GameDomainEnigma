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
#include <nana/gui/widgets/treebox.hpp>
#include "Frameworks/EventSubscriber.h"

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

        void subscribeHandlers();
        void unsubscribeHandlers();

    protected:
        void refreshWorldMapAssets();
        void refreshTerrainAssets();
        void refreshNodeAssets();

        void onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e);
        void onSpatialConstituted(const Enigma::Frameworks::IEventPtr& e);

    protected:
        MainForm* m_mainForm;
        nana::place* m_place;
        nana::treebox* m_assetsTree;

        Enigma::Frameworks::EventSubscriberPtr m_onWorldMapCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onSpatialConstituted;
    };
}

#endif // ASSETS_BROWSE_PANEL_H

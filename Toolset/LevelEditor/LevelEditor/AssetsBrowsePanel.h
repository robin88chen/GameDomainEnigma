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
#include <nana/gui/widgets/menu.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include "Frameworks/EventSubscriber.h"
#include "AssetIdCombo.h"

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

        nana::treebox* getAssetsTree() const { return m_assetsTree; }
        bool isAssetHovered() const;

        AssetIdCombo getSelectedAssetId() const;

    protected:
        void refreshWorldMapAssets();
        void refreshTerrainAssets();
        void refreshNodeAssets();

        void onAssetsTreeMouseDown(const nana::arg_mouse& arg);
        void onRemoveAsset(nana::menu::item_proxy& item);

        void onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e);
        void onSpatialConstituted(const Enigma::Frameworks::IEventPtr& e);
        void onSpatialRemoved(const Enigma::Frameworks::IEventPtr& e);

        std::string makeWorldMapAssetKey(const Enigma::WorldMap::WorldMapId& world_map_id) const;
        std::string makeTerrainAssetKey(const Enigma::SceneGraph::SpatialId& spatial_id) const;
        std::string makeNodeAssetKey(const Enigma::SceneGraph::SpatialId& spatial_id) const;

        bool isRootItemOfAssets(const nana::treebox::item_proxy& item) const;

    protected:
        MainForm* m_mainForm;
        nana::place* m_place;
        nana::treebox* m_assetsTree;
        nana::menu* m_popupMenu;

        Enigma::Frameworks::EventSubscriberPtr m_onWorldMapCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onSpatialConstituted;
        Enigma::Frameworks::EventSubscriberPtr m_onSpatialRemoved;
    };
}

#endif // ASSETS_BROWSE_PANEL_H

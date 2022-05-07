#ifndef _ASSET_PACKAGE_PANEL_H
#define _ASSET_PACKAGE_PANEL_H

#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/button.hpp"
#include "nana/gui/widgets/listbox.hpp"
#include "nana/gui/widgets/menu.hpp"
#include "AssetPackageFile.h"

namespace AssetPackageTool
{
    class AssetPackagePanel : public nana::panel<false>
    {
    public:
        AssetPackagePanel(nana::window wd, Enigma::AssetPackage::AssetPackageFile* package);
        virtual ~AssetPackagePanel();

        void Initialize();

    private:
        void OnBrowseRootButton(const nana::arg_click& ev);
        void OnAddFilesButton(const nana::arg_click& ev);
        void OnAddDirectoryButton(const nana::arg_click& ev);
        void OnExtractSelectedAsset(nana::menu::item_proxy& menu_item);
        void OnDeleteSelectedAsset(nana::menu::item_proxy& menu_item);

        void AddPackageFile(const std::string& filepath);
        std::string SplitAssetKeyName(const std::string& filepath);
        void RefreshAssetsList();
    private:
        nana::place* m_place;
        nana::label* m_rootDirLabel;
        nana::button* m_browseButton;
        nana::listbox* m_assetListbox;
        nana::button* m_addFilesButton;
        nana::button* m_addDirButton;
        nana::label* m_rootDirPrompt;

        nana::menu* m_assetSelectedMenu;

        Enigma::AssetPackage::AssetPackageFile* m_packageFile;
    };

}
#endif
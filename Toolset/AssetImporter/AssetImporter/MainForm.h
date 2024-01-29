/*********************************************************************
 * \file   MainForm.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef MAIN_FORM_H
#define MAIN_FORM_H

#include "nana/gui.hpp"
#include "nana/gui/widgets/menubar.hpp"
#include "nana/gui/widgets/tabbar.hpp"
#include "nana/gui/widgets/listbox.hpp"
#include "TextureFileStore.h"

namespace AssetImporter
{
    class MainForm : public nana::form
    {
    private:
        enum class ImportType
        {
            none,
            texture,
            effect
        };
    public:
        MainForm();
        ~MainForm();

        void initialize();

    private:
        void initFileSystem();
        void initMenu();
        void initAssetList();

        void handleClose(nana::menu::item_proxy& menu_item);
        void handleOpenTextureStorage(nana::menu::item_proxy& menu_item);
        void handleOpenEffectStorage(nana::menu::item_proxy& menu_item);
        void handleImportAsset(nana::menu::item_proxy& menu_item);

        void refreshTextureAssetList();
        void importTextureAsset();

        std::string filePathOnApkPath(const std::filesystem::path& file_path);
    private:
        ImportType m_importType;
        nana::place* m_place;
        nana::menubar* m_menubar;
        nana::tabbar<int>* m_tabbar;
        nana::listbox* m_assetListbox;
        TextureFileStore* m_textureFileStoreMapper;
    };
}

#endif // MAIN_FORM_H

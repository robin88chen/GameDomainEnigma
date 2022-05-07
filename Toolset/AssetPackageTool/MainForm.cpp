#include "MainForm.h"
#include "nana/gui/widgets/tabbar.hpp"
#include "AssetPackagePanel.h"
#include "nana/gui/filebox.hpp"
#include "AssetPackageFile.h"

using namespace AssetPackageTool;
using namespace Enigma::AssetPackage;

MainForm::MainForm()
{
    m_menubar = nullptr;
    m_place = nullptr;
    m_tabbar = nullptr;
    m_assetPanels.clear();
}

MainForm::~MainForm()
{
    delete m_menubar;
    delete m_place;
    delete m_tabbar;
    for (AssetPackagePanel* p : m_assetPanels)
    {
        delete p;
    }
    m_assetPanels.clear();
}

void MainForm::Initialize()
{
    m_place = new nana::place{ *this };
    m_place->div("vert<menubar weight=28><tabbar weight=28><tabframe>");
    InitMenu();
    m_tabbar = new nana::tabbar<int>{ *this };
    m_place->field("tabbar") << *m_tabbar;

    m_place->collocate();
}

void MainForm::InitMenu()
{
    m_menubar = new nana::menubar{ *this };
    m_menubar->push_back("&FILE");
    m_menubar->at(0).append("Create Package", [=] (auto item) { OnCreatePackage(item); });
    m_menubar->at(0).append("Open Package", [=] (auto item) { OnOpenPackage(item); });
    m_menubar->at(0).append_splitter();
    m_menubar->at(0).append("Exit", [=](auto item) { OnCloseCommand(item); });
    m_place->field("menubar") << *m_menubar;
}

void MainForm::OnCloseCommand(nana::menu::item_proxy& menu_item)
{
    close();
}
void MainForm::OnCreatePackage(nana::menu::item_proxy& menu_item)
{
    nana::filebox file_dlg{ *this, false };
    auto paths = file_dlg.title("Create New Package").add_filter("package file", "*.eph").show();
    if (paths.empty()) return;

    std::string baseFilename = paths[0].generic_string();
    size_t pos = baseFilename.find_last_of('.');
    baseFilename = baseFilename.substr(0, pos);
    AssetPackageFile* package = new AssetPackageFile{};
    package->CreateNewPackage(baseFilename);
    AssetPackagePanel* p = new AssetPackagePanel{ *this, package };
    p->Initialize();
    m_place->field("tabframe").fasten(*p);
    m_tabbar->append(paths[0].filename().stem(), *p);
    m_place->collocate();

    m_assetPanels.emplace_back(p);
}
void MainForm::OnOpenPackage(nana::menu::item_proxy& menu_item)
{
    nana::filebox file_dlg{ *this, true };
    auto paths = file_dlg.title("Open Package").add_filter("package file", "*.eph").show();
    if (paths.empty()) return;

    std::string baseFilename = paths[0].generic_string();
    size_t pos = baseFilename.find_last_of('.');
    baseFilename = baseFilename.substr(0, pos);

    AssetPackageFile* package = new AssetPackageFile{};
    package->OpenPackage(baseFilename);
    AssetPackagePanel* p = new AssetPackagePanel{ *this, package };
    p->Initialize();
    m_place->field("tabframe").fasten(*p);
    m_tabbar->append(paths[0].filename().stem(), *p);
    m_place->collocate();

    m_assetPanels.emplace_back(p);
}
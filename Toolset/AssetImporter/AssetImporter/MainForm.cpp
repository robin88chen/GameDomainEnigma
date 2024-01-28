#include "MainForm.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Gateways/DtoJsonGateway.h"

using namespace AssetImporter;
using namespace Enigma::FileSystem;
using namespace Enigma::Gateways;

MainForm::MainForm()
{
    m_menubar = nullptr;
    m_place = nullptr;
    m_tabbar = nullptr;
    m_assetListbox = nullptr;
    m_textureFileStoreMapper = nullptr;
}

MainForm::~MainForm()
{
    SAFE_DELETE(m_menubar);
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_tabbar);
    SAFE_DELETE(m_assetListbox);
    SAFE_DELETE(m_textureFileStoreMapper);

    delete FileSystem::instance();
}

void MainForm::initialize()
{
    initFileSystem();

    UISchemeColors::ApplySchemaColors(scheme());
    m_place = new nana::place{ *this };
    m_place->div("vert<menubar weight=28><<asset_list weight=360><vert<tabbar weight=28><tabframe>>>");
    initMenu();
    initAssetList();
    m_tabbar = new nana::tabbar<int>{ *this };
    UISchemeColors::ApplySchemaColors(m_tabbar->scheme());
    m_place->field("tabbar") << *m_tabbar;

    m_place->collocate();
}

void MainForm::initFileSystem()
{
    FileSystem::create();
    if (FileSystem::instance())
    {
        auto path = std::filesystem::current_path();
        auto mediaPath = path / "../../../Media/";
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(mediaPath.string(), "APK_PATH"));
        FileSystem::instance()->addMountPath(std::make_shared<StdMountPath>(path.string(), "DataPath"));
    }
}
void MainForm::initMenu()
{
    m_menubar = new nana::menubar{ *this };
    m_menubar->scheme().background = UISchemeColors::BACKGROUND;
    m_menubar->scheme().body_selected = UISchemeColors::SELECT_BG;
    m_menubar->scheme().body_highlight = UISchemeColors::HIGHLIGHT_BG;
    m_menubar->scheme().text_fgcolor = UISchemeColors::FOREGROUND;
    m_menubar->push_back("&FILE");
    m_menubar->at(0).append("Open Texture Storage", [=](auto item) { handleOpenTextureStorage(item); });
    m_menubar->at(0).append("Open Effect Storage", [=](auto item) { handleOpenEffectStorage(item); });
    //    m_menubar->at(0).append("Open Package", [=](auto item) { OnOpenPackage(item); });
    m_menubar->at(0).append_splitter();
    m_menubar->at(0).append("Exit", [=](auto item) { handleClose(item); });
    m_place->field("menubar") << *m_menubar;
}

void MainForm::initAssetList()
{
    m_assetListbox = new nana::listbox{ *this };
    m_assetListbox->append_header("Name", 120);
    m_assetListbox->append_header("Path", 240);
    m_place->field("asset_list") << *m_assetListbox;
    m_place->collocate();
}

void MainForm::handleClose(nana::menu::item_proxy& menu_item)
{
    if (m_textureFileStoreMapper)
    {
        m_textureFileStoreMapper->disconnect();
        SAFE_DELETE(m_textureFileStoreMapper);
    }
    close();
}

void MainForm::handleOpenTextureStorage(nana::menu::item_proxy& menu_item)
{
    m_menubar->at(0).enabled(1, false); // disable effect storage
    m_textureFileStoreMapper = new Enigma::FileStorage::TextureFileStoreMapper{ "textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>() };
    m_textureFileStoreMapper->connect();
}

void MainForm::handleOpenEffectStorage(nana::menu::item_proxy& menu_item)
{
    m_menubar->at(0).enabled(0, false); // disable texture storage
}

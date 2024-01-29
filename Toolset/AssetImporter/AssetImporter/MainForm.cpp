#include "MainForm.h"
#include "SchemeColorDef.h"
#include "TextureFileStore.h"
#include "Platforms/MemoryMacro.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/StdMountPath.h"
#include "Gateways/DtoJsonGateway.h"
#include "GameEngine/TextureDto.h"
#include "GameEngine/Texture.h"
#include "nana/gui/filebox.hpp"

using namespace AssetImporter;
using namespace Enigma::FileSystem;
using namespace Enigma::Gateways;

namespace fs = std::filesystem;

MainForm::MainForm()
{
    m_importType = ImportType::none;
    m_menubar = nullptr;
    m_place = nullptr;
    m_tabbar = nullptr;
    m_assetListbox = nullptr;
    m_textureFileStoreMapper = nullptr;
    m_effectFileStoreMapper = nullptr;
}

MainForm::~MainForm()
{
    SAFE_DELETE(m_menubar);
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_tabbar);
    SAFE_DELETE(m_assetListbox);
    if (m_textureFileStoreMapper)
    {
        m_textureFileStoreMapper->disconnect();
        SAFE_DELETE(m_textureFileStoreMapper);
    }
    if (m_effectFileStoreMapper)
    {
        m_effectFileStoreMapper->disconnect();
        SAFE_DELETE(m_effectFileStoreMapper);
    }
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
    m_menubar->at(0).append_splitter();
    m_menubar->at(0).append("Exit", [=](auto item) { handleClose(item); });
    m_menubar->push_back("&Import");
    m_menubar->at(1).append("Import Asset", [=](auto item) { handleImportAsset(item); });
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
    m_importType = ImportType::texture;
    m_menubar->at(0).enabled(1, false); // disable effect storage
    m_menubar->at(1).text(0, "Import Texture");
    m_textureFileStoreMapper = new TextureFileStore{ "textures.db.txt@APK_PATH", std::make_shared<DtoJsonGateway>() };
    m_textureFileStoreMapper->connect();
    refreshTextureAssetList();
}

void MainForm::handleOpenEffectStorage(nana::menu::item_proxy& menu_item)
{
    m_importType = ImportType::effect;
    m_menubar->at(0).enabled(0, false); // disable texture storage
    m_menubar->at(1).text(0, "Import Effect");
    m_effectFileStoreMapper = new EffectFileStore{ "effect_materials.db.txt@APK_PATH" };
    m_effectFileStoreMapper->connect();
    refreshEffectAssetList();
}

void MainForm::handleImportAsset(nana::menu::item_proxy& menu_item)
{
    if (m_importType == ImportType::texture)
    {
        importTextureAsset();
    }
    else if (m_importType == ImportType::effect)
    {
        importEffectAsset();
    }
}

void MainForm::refreshTextureAssetList()
{
    if (!m_textureFileStoreMapper) return;
    m_assetListbox->clear();
    auto categ = m_assetListbox->at(0);
    for (auto& [id, filename] : m_textureFileStoreMapper->filenameMap())
    {
        categ->append({ id.name(), filename });
    }
}

void MainForm::importTextureAsset()
{
    nana::filebox file_dlg{ *this, true };
    auto paths = file_dlg.add_filter({ {"PNG File(*.png)", "*.png"},{"Bitmap File(*.bmp)", "*.bmp"} }).title("Import Texture").allow_multi_select(true).show();
    if (paths.empty()) return;
    for (auto& filepath : paths)
    {
        if (fs::is_regular_file(filepath))
        {
            auto sub_path_filename = filePathOnApkPath(filepath);
            Enigma::Engine::TextureDto dto;
            auto dot_pos = sub_path_filename.find_last_of('.');
            auto texture_id = sub_path_filename.substr(0, dot_pos);
            auto asset_filename = texture_id + ".tex@APK_PATH";
            auto image_filename = sub_path_filename + "@APK_PATH";
            nana::paint::image img{ filepath.generic_string() };
            dto.id() = texture_id;
            dto.factoryDesc() = Enigma::Engine::FactoryDesc(Enigma::Engine::Texture::TYPE_RTTI.getName()).ClaimAsResourceAsset(texture_id, asset_filename);
            dto.format() = Enigma::Graphics::GraphicFormat::FMT_A8R8G8B8;
            dto.dimension() = Enigma::MathLib::Dimension<unsigned>{ img.size().width, img.size().height };
            dto.isCubeTexture() = false;
            dto.surfaceCount() = 1;
            dto.filePaths().push_back(image_filename);
            m_textureFileStoreMapper->putTexture(dto.id(), dto.toGenericDto());
        }
    }
    refreshTextureAssetList();
}

void MainForm::refreshEffectAssetList()
{
    if (!m_effectFileStoreMapper) return;
    m_assetListbox->clear();
    auto categ = m_assetListbox->at(0);
    for (auto& [id, filename] : m_effectFileStoreMapper->filenameMap())
    {
        categ->append({ id.name(), filename });
    }
}

void MainForm::importEffectAsset()
{
    nana::filebox file_dlg{ *this, true };
    auto paths = file_dlg.add_filter({ {"Effect File(*.efx)", "*.efx"} }).title("Import Effect").allow_multi_select(true).show();
    if (paths.empty()) return;
    for (auto& filepath : paths)
    {
        if (fs::is_regular_file(filepath))
        {
            auto sub_path_filename = filePathOnApkPath(filepath);
            auto dot_pos = sub_path_filename.find_last_of('.');
            auto material_id = sub_path_filename.substr(0, dot_pos);
            auto asset_filename = sub_path_filename + "@APK_PATH";
            m_effectFileStoreMapper->appendEffectMaterial(material_id, asset_filename);
        }
    }
    m_effectFileStoreMapper->serializeMapperFile();
    refreshEffectAssetList();
}

std::string MainForm::filePathOnApkPath(const std::filesystem::path& file_path)
{
    const std::list<IMountPathPtr> mount_paths = FileSystem::instance()->getMountPathsWithPathId("APK_PATH");
    if (mount_paths.empty()) return file_path.filename().string();
    std::filesystem::path parent_path = file_path.parent_path().parent_path(); // 暫時只往上一層尋找
    for (auto mp : mount_paths)
    {
        if (mp->equalMountPath(parent_path))
        {
            return (--(--file_path.end()))->string() + "/" + file_path.filename().string(); // +"@" + mp->GetPathID();
        }
    }
    return file_path.filename().string();
}

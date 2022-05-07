#include "AssetPackagePanel.h"
#include "nana/gui/filebox.hpp"
#include "AssetNameList.h"
#include <filesystem>

using namespace Enigma::AssetPackage;

namespace fs = std::filesystem;
void AssetVersionToTimeValue(unsigned int ver, int* year, int* month, int* day, int* hour, int* minute)
{
    if (minute)
    {
        *minute = ver & 0xf;
    }
    ver = ver >> 4;
    if (hour)
    {
        *hour = ver & 0x1f;
    }
    ver = ver >> 5;
    if (day)
    {
        *day = ver & 0x1f;
    }
    ver = ver >> 5;
    if (month)
    {
        *month = (ver & 0xf) + 1;
    }
    ver = ver >> 4;
    if (year)
    {
        *year = ver + 2010;
    }
}

AssetPackageTool::AssetPackagePanel::AssetPackagePanel(nana::window wd, AssetPackageFile* package)
    : nana::panel<false>{wd}, m_packageFile { package }
{
    m_place = nullptr;
    m_rootDirLabel = nullptr;
    m_browseButton = nullptr;
    m_assetListbox = nullptr;
    m_addFilesButton = nullptr;
    m_addDirButton = nullptr;
    m_rootDirPrompt = nullptr;
    m_assetSelectedMenu = nullptr;
}

AssetPackageTool::AssetPackagePanel::~AssetPackagePanel()
{
    delete m_place;
    delete m_rootDirLabel;
    delete m_browseButton;
    delete m_assetListbox;
    delete m_addFilesButton;
    delete m_addDirButton;
    delete m_rootDirPrompt;
    delete m_packageFile;
    delete m_assetSelectedMenu;
}

void AssetPackageTool::AssetPackagePanel::Initialize()
{
    m_place = new nana::place{ *this };
    m_place->div("<leftpart vert< row_one weight=28 arrange=[80,variable,80] > < row_two weight=28 > >"
        "|<rightpart weight=70%>");
    m_rootDirPrompt = new nana::label { *this, "Root Dir" };
    m_rootDirLabel = new nana::label{ *this, "Test" };
    m_browseButton = new nana::button{ *this, "Browse" };
    m_browseButton->events().click([=] (const nana::arg_click& a)
        { this->OnBrowseRootButton(a); });  // a 的 type 用 auto 會編譯失敗， why??
    (*m_place)["row_one"] << *m_rootDirPrompt << *m_rootDirLabel << *m_browseButton;
    m_addFilesButton = new nana::button{ *this, "Add Files" };
    m_addFilesButton->events().click([=] (const nana::arg_click& a)
        { this->OnAddFilesButton(a); });  // a 的 type 用 auto 會編譯失敗， why??
    m_addDirButton = new nana::button{ *this, "Add Dir" };
    m_addDirButton->events().click([=] (const nana::arg_click& a)
        { this->OnAddDirectoryButton(a); });  // a 的 type 用 auto 會編譯失敗， why??
    (*m_place)["row_two"] << *m_addDirButton << *m_addFilesButton;
    m_assetListbox = new nana::listbox{ *this };
    m_assetListbox->append_header("Name");
    m_assetListbox->append_header("Offset");
    m_assetListbox->append_header("Size");
    m_assetListbox->append_header("Version");
    m_assetListbox->append_header("Time Stamp");
    m_assetListbox->append_header("CRC");
    (*m_place)["rightpart"] << *m_assetListbox;
    m_place->collocate();

    m_assetSelectedMenu = new nana::menu{};
    m_assetSelectedMenu->append("Extract", [=] (auto item) { this->OnExtractSelectedAsset(item); });
    m_assetListbox->events().mouse_down(menu_popuper(*m_assetSelectedMenu));

    RefreshAssetsList();
}

void AssetPackageTool::AssetPackagePanel::OnBrowseRootButton(const nana::arg_click& ev)
{
    nana::folderbox picker;

    auto paths = picker.show();
    if (paths.empty()) return;
    if (m_rootDirLabel)
    {
        m_rootDirLabel->caption(paths[0].generic_string());
    }
}

void AssetPackageTool::AssetPackagePanel::OnAddFilesButton(const nana::arg_click& ev)
{
    nana::filebox file_dlg{ *this, true };
    auto paths = file_dlg.title("Add Package Files").allow_multi_select(true).show();
    if (paths.empty()) return;
    for (auto& filepath : paths)
    {
        if (fs::is_regular_file(filepath))
        {
            AddPackageFile(filepath.generic_string());
        }
    }
    RefreshAssetsList();
}

void AssetPackageTool::AssetPackagePanel::OnAddDirectoryButton(const nana::arg_click& ev)
{
    nana::folderbox folder_dlg{ *this };
    auto paths = folder_dlg.show();
    if (paths.empty()) return;
    for (auto& p : fs::directory_iterator{ paths[0] })
    {
        if (fs::is_regular_file(p.path()))
        {
            AddPackageFile(p.path().generic_string());
        }
    }
    RefreshAssetsList();
}

void AssetPackageTool::AssetPackagePanel::OnExtractSelectedAsset(nana::menu::item_proxy& menu_item)
{
    if (!m_assetListbox) return;
    auto selected_item = m_assetListbox->selected();
    if (selected_item.empty()) return;
    std::string asset_key = m_assetListbox->at(selected_item[0].cat).at(selected_item[0].item).text(0);
    if (!m_packageFile) return;
    nana::filebox save_dlg{ *this, false };
    auto paths = save_dlg.init_file(asset_key).title("Extract Asset to File").show();
    if (paths.empty()) return;
    m_packageFile->TryRetrieveAssetToFile(paths[0].generic_string(), asset_key);
}

void AssetPackageTool::AssetPackagePanel::AddPackageFile(const std::string& filepath)
{
    if (filepath.empty()) return;
    std::string asset_key = SplitAssetKeyName(filepath);
    m_packageFile->AddAssetFile(filepath, asset_key, AssetPackageFile::VERSION_USE_FILE_TIME);
}

std::string AssetPackageTool::AssetPackagePanel::SplitAssetKeyName(const std::string& filepath)
{
    size_t str_pos = filepath.find_last_of('/');
    std::string asset_key;
    if (str_pos == std::string::npos)
    {
        asset_key = filepath;
        return asset_key;
    }
    asset_key = filepath.substr(str_pos + 1);
    if (!m_rootDirLabel) return asset_key;
    if (m_rootDirLabel->caption().empty()) return asset_key;
    str_pos = filepath.find(m_rootDirLabel->caption());
    if (str_pos != 0) // 不是從頭，或是沒找到
    {
        return asset_key;
    }
    asset_key = filepath.substr(m_rootDirLabel->caption().length());
    if (asset_key[0] == '/')
    {
        asset_key = asset_key.substr(1);
    }
    return asset_key;
}

void AssetPackageTool::AssetPackagePanel::RefreshAssetsList()
{
    if (!m_packageFile) return;
    if (!m_assetListbox) return;
    m_assetListbox->clear();

    const std::unique_ptr<AssetNameList>& nameList = m_packageFile->GetAssetNameList();
    if (!nameList) return;
    auto nameArray = nameList->GetAssetNameArray();
    for (std::string key : nameArray)
    {
        auto header_data = m_packageFile->TryGetAssetHeaderData(key);
        if (!header_data) continue;
        // stringstream 的格式化字串很不好用
        char ss[1024];
        sprintf(ss, "%d", header_data->m_offset);
        std::string offsetToken{ ss };
        sprintf(ss, "%d/%d", header_data->m_size, header_data->m_orgSize);
        std::string sizeToken{ ss };
        sprintf(ss, "%08x", header_data->m_version);
        std::string verToken{ ss };
        int year, month, day, hour, minute;
        AssetVersionToTimeValue(header_data->m_version, &year, &month, &day, &hour, &minute);
        sprintf(ss, "%d:%02d:%02d:%02d:%02d", year, month, day, hour, minute);
        std::string timeToken{ ss };
        sprintf(ss, "%08x", header_data->m_crc);
        std::string crcToken{ ss };

        auto categ = m_assetListbox->at(0);
        categ.append({ key, offsetToken, sizeToken, verToken, timeToken, crcToken });
    }
}

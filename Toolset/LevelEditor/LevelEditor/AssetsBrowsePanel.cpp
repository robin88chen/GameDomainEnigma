#include "AssetsBrowsePanel.h"
#include "Platforms/MemoryMacro.h"

using namespace LevelEditor;
AssetsBrowsePanel::AssetsBrowsePanel(const nana::window& wd) : panel<false>{ wd }
{

}

AssetsBrowsePanel::~AssetsBrowsePanel()
{

}

void AssetsBrowsePanel::initialize(MainForm* main_form)
{
    m_mainForm = main_form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<asset_list>");
    m_assetsList = menew nana::listbox{ *this };
    (*m_place)["asset_list"] << *m_assetsList;

    m_place->collocate();
}

void AssetsBrowsePanel::finalize()
{
}


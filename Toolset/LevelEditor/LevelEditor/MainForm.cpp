#include "MainForm.h"
#include "Platforms/MemoryMacro.h"
#include "SchemeColorDef.h"
#include "LevelEditorAppDelegate.h"
#include "RenderPanel.h"
#include "SceneGraphPanel.h"
#include "SpatialInspectorToolPanel.h"
#include "TerrainToolPanel.h"
#include "OutputPanel.h"

using namespace LevelEditor;
using namespace Enigma::Graphics;
using namespace std::chrono_literals;

MainForm::MainForm() : nana::form()
{
    m_menubar = nullptr;
    m_tabbar = nullptr;
    m_toolbar = nullptr;
    m_timer = nullptr;
    m_renderPanel = nullptr;
    m_sceneGraphPanel = nullptr;
    m_spatialInspectorPanel = nullptr;
    m_terrainToolPanel = nullptr;
    m_outputPanel = nullptr;
}

MainForm::~MainForm()
{
    SAFE_DELETE(m_menubar);
    SAFE_DELETE(m_tabbar);
    SAFE_DELETE(m_toolbar);
    SAFE_DELETE(m_statusCameraZone);
    SAFE_DELETE(m_timer);
    SAFE_DELETE(m_renderPanel);
    SAFE_DELETE(m_sceneGraphPanel);
    SAFE_DELETE(m_spatialInspectorPanel);
    SAFE_DELETE(m_appDelegate);
    SAFE_DELETE(m_terrainToolPanel);
    SAFE_DELETE(m_outputPanel);
}

void MainForm::InitSubPanels()
{
    UISchemeColors::ApplySchemaColors(scheme());
    get_place().div("vert<menubar weight=28><main_tools weight=28>< <scene_graph_panel weight=15%> | <render_panel weight=70%> | <vert<toolsbar weight=28> <toolsframe> > > <outputpanel weight=100><statuspanel weight=16>");
    InitMenu();
    m_tabbar = menew nana::tabbar<int>{ *this };
    UISchemeColors::ApplySchemaColors(m_tabbar->scheme());
    get_place().field("toolsbar") << *m_tabbar;
    InitPanels();
    m_toolbar = menew nana::toolbar(*this);
    get_place().field("main_tools") << *m_toolbar;
    InitTools();
    m_statusCameraZone = menew nana::label(*this, "Camera Zone : ");
    get_place().field("statuspanel") << *m_statusCameraZone;

    InitializeGraphics();

    get_place().collocate();
}

void MainForm::InitializeGraphics()
{
    m_appDelegate = new EditorAppDelegate();
    m_appDelegate->Initialize(IGraphicAPI::APIVersion::API_Dx11, IGraphicAPI::AsyncType::UseAsyncDevice, "viewer_log.log",
        reinterpret_cast<HWND>(m_renderPanel->native_handle()));
    m_timer = new nana::timer{ 1ms };
    m_timer->elapse([this] { m_appDelegate->OnTimerElapsed(); });
    m_timer->start();
    events().destroy([this] { this->FinalizeGraphics(); });
}

void MainForm::FinalizeGraphics()
{
    if (m_appDelegate) m_appDelegate->Finalize();
}

void MainForm::InitMenu()
{
    m_menubar = menew nana::menubar{ *this };
    m_menubar->scheme().background = UISchemeColors::BACKGROUND;
    m_menubar->scheme().body_selected = UISchemeColors::SELECT_BG;
    m_menubar->scheme().body_highlight = UISchemeColors::HIGHLIGHT_BG;
    m_menubar->scheme().text_fgcolor = UISchemeColors::FOREGROUND;
    nana::menu& file_menu = m_menubar->push_back("&File");
    file_menu.append("Save World Map", [this](auto item) { OnSaveWorldCommand(item); });
    file_menu.append("Load World Map", [this](auto item) { OnLoadWorldCommand(item); });
    file_menu.append_splitter();
    file_menu.append("Exit", [this](auto item) { OnCloseCommand(item); });

    nana::menu& map_menu = m_menubar->push_back("&World Map");
    map_menu.append("Create New Map", [this](auto item) { OnCreateWorldMapCommand(item); });
    map_menu.append_splitter();
    map_menu.append("Add Terrain", [this](auto item) { OnAddTerrainCommand(item); });
    map_menu.append("Add Environment Light", [this](auto item) { OnAddEnviromentLightCommand(item); });

    nana::menu& portal_menu = m_menubar->push_back("&Portal");
    portal_menu.append("Create Zone Node...", [this](auto item) { OnCreateZoneNodeCommand(item); });
    portal_menu.append("Add Portal...", [this](auto item) { OnAddPortalCommand(item); });

    get_place().field("menubar") << *m_menubar;
}

void MainForm::InitPanels()
{
    m_renderPanel = menew RenderPanel{ *this };
    get_place().field("render_panel") << *m_renderPanel;

    m_sceneGraphPanel = menew SceneGraphPanel{ *this };
    m_sceneGraphPanel->Initialize(this);
    get_place().field("scene_graph_panel").fasten(*m_sceneGraphPanel);

    m_spatialInspectorPanel = menew SpatialInspectorPanel{ *this };
    m_spatialInspectorPanel->Initialize(this);
    get_place().field("toolsframe").fasten(*m_spatialInspectorPanel);
    m_tabbar->append("Inspector", *m_spatialInspectorPanel);
    m_terrainToolPanel = menew TerrainToolPanel{ *this };
    m_terrainToolPanel->Initialize(this);
    get_place().field("toolsframe").fasten(*m_terrainToolPanel);
    m_tabbar->append("Terrain", *m_terrainToolPanel);

    m_outputPanel = menew OutputPanel{ *this };
    m_outputPanel->Initialize();
    get_place()["outputpanel"] << *m_outputPanel;
}

void MainForm::InitTools()
{
    m_toolbar->scheme().background = UISchemeColors::BACKGROUND;
    m_toolbar->scheme().foreground = UISchemeColors::FOREGROUND;
    m_toolbar->scheme().activated = UISchemeColors::SELECT_BG;
    m_toolbar->append(nana::toolbar::tools::toggle, "Cursor Mode",
        nana::paint::image("icons/cursor_arrow.bmp")).toggle_group("select_mode").toggle(true);
    m_toolbar->append(nana::toolbar::tools::toggle, "Edit Terrain Mode",
        nana::paint::image("icons/edit_terrain.bmp")).toggle_group("select_mode");
    m_toolbar->append(nana::toolbar::tools::toggle, "Edit Entity Mode",
        nana::paint::image("icons/creature_entity.bmp")).toggle_group("select_mode");
    m_toolbar->append_separator();
    m_toolbar->append(nana::toolbar::tools::toggle, "Move Entity",
        nana::paint::image("icons/move.bmp")).toggle_group("entity_mode").toggle(true);
    m_toolbar->append(nana::toolbar::tools::toggle, "Rotate Entity",
        nana::paint::image("icons/rotate.bmp")).toggle_group("entity_mode");
    m_toolbar->append(nana::toolbar::tools::toggle, "Scale Entity",
        nana::paint::image("icons/scale.bmp")).toggle_group("entity_mode");
    m_toolbar->append_separator();
    m_toolbar->append(nana::toolbar::tools::dropdown, "Candidate Entities",
        nana::paint::image("icons/creature_entity.bmp")).dropdown_append(
            "...", [this](const nana::toolbar::item_proxy& it) { this->OnAddCandidateEntity(it); });
    m_toolbar->append_separator();
    m_toolbar->append(nana::toolbar::tools::toggle, "God Mode",
        nana::paint::image("icons/god_mode.bmp")).toggle_group("god_mode");
    m_toolbar->append_separator();
    m_toolbar->append(nana::toolbar::tools::dropdown, "Select Zone Node", nana::paint::image());

    m_toolbar->at((size_t)ToolIndex::ToolCandidateEntity).textout(true);
    m_toolbar->at((size_t)ToolIndex::ToolZoneSelect).textout(true);
    m_toolbar->events().selected([this](const nana::arg_toolbar& a) { this->OnToolBarSelected(a); });
}

void MainForm::OnCloseCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnCreateWorldMapCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnLoadWorldCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnSaveWorldCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnAddTerrainCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnAddEnviromentLightCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnSelectEntity(const nana::toolbar::item_proxy& drop_down_item, const std::string& entity_name)
{

}

void MainForm::OnCreateZoneNodeCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnAddPortalCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnAddCandidateEntity(const nana::toolbar::item_proxy& drop_down_item)
{

}

void MainForm::OnGodModeChanged(bool enabled)
{

}

void MainForm::OnToolBarSelected(const nana::arg_toolbar& arg)
{

}

void MainForm::OnSelectZoneNode(const nana::toolbar::item_proxy& drop_down_item, const std::string& node_name)
{

}

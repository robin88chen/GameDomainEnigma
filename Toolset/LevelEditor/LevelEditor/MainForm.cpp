#include "MainForm.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "FileSystem/FileSystem.h"
#include "AddTerrainDialog.h"
#include "CreateNewWorldDlg.h"
#include "Platforms/MemoryMacro.h"
#include "SchemeColorDef.h"
#include "LevelEditorAppDelegate.h"
#include "RenderPanel.h"
#include "SceneGraphPanel.h"
#include "SpatialInspectorToolPanel.h"
#include "TerrainToolPanel.h"
#include "OutputPanel.h"
#include "WorldEditConsole.h"
#include "AppConfiguration.h"
#include "LevelEditorCommands.h"
#include "WorldEditService.h"
#include "TerrainEditConsole.h"
#include "WorldMap/WorldMapService.h"
#include "Gateways/DtoJsonGateway.h"
#include "LevelEditorEvents.h"
#include "nana/gui/filebox.hpp"

using namespace LevelEditor;
using namespace Enigma::Graphics;
using namespace Enigma::WorldMap;
using namespace Enigma::Gateways;
using namespace Enigma::Engine;
using namespace Enigma::FileSystem;
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
    m_editorMode = EditorMode::Cursor;
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

    if (m_sceneGraphPanel) m_sceneGraphPanel->SubscribeHandlers();
    if (m_outputPanel) m_outputPanel->SubscribeHandlers();
    if (m_renderPanel)
    {
        m_renderPanel->InitInputHandler(m_appDelegate->GetInputHandler());
        m_renderPanel->SubscribeHandlers();
    }
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
    auto srv_mngr = Enigma::Controllers::GraphicMain::Instance()->GetServiceManager();
    auto world_edit = std::dynamic_pointer_cast<WorldEditService, Enigma::Frameworks::ISystemService>(srv_mngr->GetSystemService(WorldEditService::TYPE_RTTI));
    srv_mngr->RegisterSystemService(std::make_shared<WorldEditConsole>(srv_mngr, world_edit));
    srv_mngr->RegisterSystemService(std::make_shared<TerrainEditConsole>(srv_mngr));
    m_worldConsole = srv_mngr->GetSystemServiceAs<WorldEditConsole>();
    m_worldConsole.lock()->SetWorldMapRootFolder(m_appDelegate->GetAppConfig()->GetWorldMapRootFolderName(), m_appDelegate->GetAppConfig()->GetWorldMapPathId());
}

void MainForm::FinalizeGraphics()
{
    auto srv_mngr = Enigma::Controllers::GraphicMain::Instance()->GetServiceManager();
    srv_mngr->UnregisterSystemService(WorldEditConsole::TYPE_RTTI);
    srv_mngr->UnregisterSystemService(TerrainEditConsole::TYPE_RTTI);

    if (m_renderPanel) m_renderPanel->UnsubscribeHandlers();
    if (m_sceneGraphPanel)
    {
        m_sceneGraphPanel->Finalize();
        m_sceneGraphPanel->UnsubscribeHandlers();
    }
    if (m_outputPanel) m_outputPanel->UnsubscribeHandlers();
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
    close();
}

void MainForm::OnCreateWorldMapCommand(const nana::menu::item_proxy& menu_item)
{
    nana::API::modal_window(CreateNewWorldDlg(*this, m_worldConsole.lock()));
}

void MainForm::OnLoadWorldCommand(const nana::menu::item_proxy& menu_item)
{
    auto srv_mngr = Enigma::Controllers::GraphicMain::Instance()->GetServiceManager();
    auto world = srv_mngr->GetSystemServiceAs<WorldMapService>();

    nana::filebox fb(handle(), true);
    fb.add_filter({ {"World File(*.wld)", "*.wld"} }).title("Load World");
    if (auto paths = fb.show(); !paths.empty())
    {
        if (!m_worldConsole.expired())
        {
            m_worldConsole.lock()->LoadWorldMap(paths[0]);
        }
    }
}

void MainForm::OnSaveWorldCommand(const nana::menu::item_proxy& menu_item)
{
    assert(!m_worldConsole.expired());
    Enigma::Frameworks::CommandBus::Post(std::make_shared<OutputMessage>("Save World File..."));
    m_worldConsole.lock()->SaveWorldMap();
}

void MainForm::OnAddTerrainCommand(const nana::menu::item_proxy& menu_item)
{
    nana::API::modal_window(AddTerrainDialog(*this, m_worldConsole.lock(), m_appDelegate->GetAppConfig()->GetMediaPathId()));
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
    switch (arg.button)
    {
    case static_cast<size_t>(ToolIndex::ToolCursor):
        Enigma::Frameworks::EventPublisher::Post(std::make_shared<EditorModeChanged>(m_editorMode, EditorMode::Cursor));
        m_editorMode = EditorMode::Cursor;
        break;
    case static_cast<size_t>(ToolIndex::ToolTerrain):
        Enigma::Frameworks::EventPublisher::Post(std::make_shared<EditorModeChanged>(m_editorMode, EditorMode::Terrain));
        m_editorMode = EditorMode::Terrain;
        break;
    case static_cast<size_t>(ToolIndex::ToolEntity):
        Enigma::Frameworks::EventPublisher::Post(std::make_shared<EditorModeChanged>(m_editorMode, EditorMode::Pawn));
        m_editorMode = EditorMode::Pawn;
        break;
    /*case static_cast<size_t>(ToolIndex::ToolMoveEntity):
        break;
    case static_cast<size_t>(ToolIndex::ToolRotateEntity):
        break;
    case static_cast<size_t>(ToolIndex::ToolScaleEntity):
        break;
    case static_cast<size_t>(ToolIndex::ToolGodMode):
        break;*/
    default:
        break;
    }
}

void MainForm::OnSelectZoneNode(const nana::toolbar::item_proxy& drop_down_item, const std::string& node_name)
{

}

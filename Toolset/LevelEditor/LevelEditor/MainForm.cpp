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
#include "TerrainEditService.h"
#include "TerrainEditConsole.h"
#include "EditorSceneConsole.h"
#include "WorldMap/WorldMapService.h"
#include "Gateways/DtoJsonGateway.h"
#include "LevelEditorEvents.h"
#include "nana/gui/filebox.hpp"
#include "FrustumInfoDialog.h"
#include "PawnEditConsole.h"
#include "PawnEditService.h"
#include "EditorUtilities.h"

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
    m_editorMode = EditorMode::cursor;
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

void MainForm::initSubPanels()
{
    UISchemeColors::applySchemaColors(scheme());
    get_place().div("vert<menubar weight=28><main_tools weight=28>< <scene_graph_panel weight=15%> | <render_panel weight=70%> | <vert<toolsbar weight=28> <toolsframe> > > <outputpanel weight=100><statuspanel weight=16>");
    initMenu();
    m_tabbar = menew nana::tabbar<int>{ *this };
    UISchemeColors::applySchemaColors(m_tabbar->scheme());
    get_place().field("toolsbar") << *m_tabbar;
    initPanels();
    m_toolbar = menew nana::toolbar(*this);
    get_place().field("main_tools") << *m_toolbar;
    initTools();
    m_statusCameraZone = menew nana::label(*this, "Camera Zone : ");
    m_statusCameraZone->scheme().foreground = UISchemeColors::FOREGROUND;
    m_statusCameraZone->scheme().background = UISchemeColors::BACKGROUND;
    get_place().field("statuspanel") << *m_statusCameraZone;

    initializeGraphics();

    get_place().collocate();

    if (m_sceneGraphPanel) m_sceneGraphPanel->subscribeHandlers();
    if (m_outputPanel) m_outputPanel->subscribeHandlers();
    if (m_renderPanel)
    {
        m_renderPanel->initInputHandler(m_appDelegate->inputHandler());
        m_renderPanel->subscribeHandlers();
    }
    if (m_spatialInspectorPanel) m_spatialInspectorPanel->subscribeHandlers();
    if (m_terrainToolPanel) m_terrainToolPanel->subscribeHandlers();
}

void MainForm::initializeGraphics()
{
    m_appDelegate = new EditorAppDelegate();
    m_appDelegate->initialize(IGraphicAPI::APIVersion::API_Dx11, IGraphicAPI::AsyncType::UseAsyncDevice, "viewer_log.log",
        reinterpret_cast<HWND>(m_renderPanel->native_handle()));
    m_timer = new nana::timer{ 1ms };
    m_timer->elapse([this] { m_appDelegate->onTimerElapsed(); });
    m_timer->start();
    events().destroy([this] { this->finalizeGraphics(); });
    auto srv_mngr = Enigma::Controllers::GraphicMain::Instance()->getServiceManager();
    auto world_edit = std::dynamic_pointer_cast<WorldEditService>(srv_mngr->getSystemService(WorldEditService::TYPE_RTTI));
    srv_mngr->registerSystemService(std::make_shared<WorldEditConsole>(srv_mngr, world_edit));
    srv_mngr->registerSystemService(std::make_shared<TerrainEditConsole>(srv_mngr));
    srv_mngr->registerSystemService(std::make_shared<EditorSceneConsole>(srv_mngr));
    auto pawn_edit = std::dynamic_pointer_cast<PawnEditService>(srv_mngr->getSystemService(PawnEditService::TYPE_RTTI));
    srv_mngr->registerSystemService(std::make_shared<PawnEditConsole>(srv_mngr, pawn_edit));
    m_worldConsole = srv_mngr->getSystemServiceAs<WorldEditConsole>();
    m_worldConsole.lock()->setWorldMapRootFolder(m_appDelegate->appConfig()->worldMapRootFolderName(), m_appDelegate->appConfig()->worldMapPathId());
    m_pawnConsole = srv_mngr->getSystemServiceAs<PawnEditConsole>();
}

void MainForm::finalizeGraphics()
{
    auto srv_mngr = Enigma::Controllers::GraphicMain::Instance()->getServiceManager();
    srv_mngr->unregisterSystemService(WorldEditConsole::TYPE_RTTI);
    srv_mngr->unregisterSystemService(TerrainEditConsole::TYPE_RTTI);
    srv_mngr->unregisterSystemService(EditorSceneConsole::TYPE_RTTI);
    srv_mngr->unregisterSystemService(PawnEditConsole::TYPE_RTTI);

    if (m_terrainToolPanel) m_terrainToolPanel->unsubscribeHandlers();
    if (m_spatialInspectorPanel) m_spatialInspectorPanel->unsubscribeHandlers();
    if (m_renderPanel) m_renderPanel->unsubscribeHandlers();
    if (m_sceneGraphPanel)
    {
        m_sceneGraphPanel->finalize();
        m_sceneGraphPanel->unsubscribeHandlers();
    }
    if (m_outputPanel) m_outputPanel->unsubscribeHandlers();
    if (m_appDelegate) m_appDelegate->finalize();
}

void MainForm::initMenu()
{
    m_menubar = menew nana::menubar{ *this };
    m_menubar->scheme().background = UISchemeColors::BACKGROUND;
    m_menubar->scheme().body_selected = UISchemeColors::SELECT_BG;
    m_menubar->scheme().body_highlight = UISchemeColors::HIGHLIGHT_BG;
    m_menubar->scheme().text_fgcolor = UISchemeColors::FOREGROUND;
    nana::menu& file_menu = m_menubar->push_back("&File");
    file_menu.append("Save World Map", [this](auto item) { onSaveWorldCommand(item); });
    file_menu.append("Load World Map", [this](auto item) { onLoadWorldCommand(item); });
    file_menu.append_splitter();
    file_menu.append("Exit", [this](auto item) { onCloseCommand(item); });

    nana::menu& map_menu = m_menubar->push_back("&World Map");
    map_menu.append("Create New Map", [this](auto item) { onCreateWorldMapCommand(item); });
    map_menu.append_splitter();
    map_menu.append("Add Terrain", [this](auto item) { onAddTerrainCommand(item); });
    map_menu.append("Add Environment Light", [this](auto item) { onAddEnvironmentLightCommand(item); });

    nana::menu& portal_menu = m_menubar->push_back("&Portal");
    portal_menu.append("Create Zone Node...", [this](auto item) { onCreateZoneNodeCommand(item); });
    portal_menu.append("Add Portal...", [this](auto item) { onAddPortalCommand(item); });

    nana::menu& option_menu = m_menubar->push_back("&Options");
    option_menu.append("Camera Frustum", [this](auto item) { onCameraFrustumCommand(item); });

    get_place().field("menubar") << *m_menubar;
}

void MainForm::initPanels()
{
    m_renderPanel = menew RenderPanel{ *this };
    get_place().field("render_panel") << *m_renderPanel;

    m_sceneGraphPanel = menew SceneGraphPanel{ *this };
    m_sceneGraphPanel->initialize(this);
    get_place().field("scene_graph_panel").fasten(*m_sceneGraphPanel);

    m_spatialInspectorPanel = menew SpatialInspectorPanel{ *this };
    m_spatialInspectorPanel->initialize(this);
    get_place().field("toolsframe").fasten(*m_spatialInspectorPanel);
    m_tabbar->append("Inspector", *m_spatialInspectorPanel);
    m_terrainToolPanel = menew TerrainToolPanel{ *this };
    m_terrainToolPanel->initialize(this, TerrainEditService::TextureLayerNum);
    get_place().field("toolsframe").fasten(*m_terrainToolPanel);
    m_tabbar->append("Terrain", *m_terrainToolPanel);

    m_outputPanel = menew OutputPanel{ *this };
    m_outputPanel->initialize();
    get_place()["outputpanel"] << *m_outputPanel;
}

void MainForm::initTools()
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
            "...", [this](const nana::toolbar::item_proxy& it) { this->onAddCandidatePawn(it); });
    m_toolbar->append_separator();
    m_toolbar->append(nana::toolbar::tools::toggle, "God Mode",
        nana::paint::image("icons/god_mode.bmp")).toggle_group("god_mode");
    m_toolbar->append_separator();
    m_toolbar->append(nana::toolbar::tools::dropdown, "Select Zone Node", nana::paint::image());

    m_toolbar->at((size_t)ToolIndex::ToolCandidateEntity).textout(true);
    m_toolbar->at((size_t)ToolIndex::ToolZoneSelect).textout(true);
    m_toolbar->events().selected([this](const nana::arg_toolbar& a) { this->onToolBarSelected(a); });
}

void MainForm::onCloseCommand(const nana::menu::item_proxy& menu_item)
{
    close();
}

void MainForm::onCreateWorldMapCommand(const nana::menu::item_proxy& menu_item)
{
    nana::API::modal_window(CreateNewWorldDlg(*this, m_worldConsole.lock(), m_appDelegate->appConfig()->portalManagementName()));
}

void MainForm::onLoadWorldCommand(const nana::menu::item_proxy& menu_item)
{
    auto srv_mngr = Enigma::Controllers::GraphicMain::Instance()->getServiceManager();
    auto world = srv_mngr->getSystemServiceAs<WorldMapService>();

    nana::filebox fb(handle(), true);
    fb.add_filter({ {"World File(*.wld)", "*.wld"} }).title("Load World");
    if (auto paths = fb.show(); !paths.empty())
    {
        if (!m_worldConsole.expired())
        {
            m_worldConsole.lock()->loadWorldMap(paths[0], m_appDelegate->appConfig()->portalManagementName());
        }
    }
}

void MainForm::onSaveWorldCommand(const nana::menu::item_proxy& menu_item)
{
    assert(!m_worldConsole.expired());
    Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("Save World File..."));
    m_worldConsole.lock()->saveWorldMap();
}

void MainForm::onAddTerrainCommand(const nana::menu::item_proxy& menu_item)
{
    nana::API::modal_window(AddTerrainDialog(*this, m_worldConsole.lock(), m_appDelegate->appConfig()->mediaPathId()));
}

void MainForm::onAddEnvironmentLightCommand(const nana::menu::item_proxy& menu_item)
{
    Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("Add Environment Light..."));
    Enigma::Frameworks::CommandBus::post(std::make_shared<CreateEnvironmentLight>(m_worldConsole.lock()->getCurrentWorldName()));
}

void MainForm::onSelectPawn(const nana::toolbar::item_proxy& drop_down_item, const std::string& pawn_name)
{
    if (m_pawnConsole.expired()) return;
    m_pawnConsole.lock()->selectCandidatePawn(pawn_name);
}

void MainForm::onCreateZoneNodeCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::onAddPortalCommand(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::onAddCandidatePawn(const nana::toolbar::item_proxy& drop_down_item)
{
    nana::filebox fb(handle(), true);
    fb.add_filter({ {"Pawn File(*.pawn)", "*.pawn"} }).title("Load Pawn");
    if (auto paths = fb.show(); !paths.empty())
    {
        Filename filename_obj(paths[0].string());
        std::string pawn_name = filename_obj.GetBaseFileName();
        Enigma::Frameworks::CommandBus::post(std::make_shared<OutputMessage>("Add Candidate Pawn " + pawn_name + " File " + paths[0].string() + "..."));
        if (!m_pawnConsole.expired())
        {
            std::string filename_sub_path = filePathCombinePathID(paths[0], m_appDelegate->appConfig()->mediaPathId());
            m_pawnConsole.lock()->insertCandidatePawnFilePath(pawn_name, filename_sub_path + "@" + m_appDelegate->appConfig()->mediaPathId());
        }
        ((nana::toolbar::item_proxy)(drop_down_item)).dropdown_append(pawn_name,
            [=](const nana::toolbar::item_proxy& it) { this->onSelectPawn(it, pawn_name); });
    }
}

void MainForm::onGodModeChanged(bool enabled)
{

}

void MainForm::onCameraFrustumCommand(const nana::menu::item_proxy& menu_item)
{
    nana::API::modal_window(FrustumInfoDialog(*this, m_appDelegate->appConfig()->cameraName()));
}

void MainForm::onToolBarSelected(const nana::arg_toolbar& arg)
{
    switch (arg.button)
    {
    case static_cast<size_t>(ToolIndex::ToolCursor):
        Enigma::Frameworks::EventPublisher::post(std::make_shared<EditorModeChanged>(m_editorMode, EditorMode::cursor));
        m_editorMode = EditorMode::cursor;
        break;
    case static_cast<size_t>(ToolIndex::ToolTerrain):
        Enigma::Frameworks::EventPublisher::post(std::make_shared<EditorModeChanged>(m_editorMode, EditorMode::terrain));
        m_editorMode = EditorMode::terrain;
        break;
    case static_cast<size_t>(ToolIndex::ToolEntity):
        Enigma::Frameworks::EventPublisher::post(std::make_shared<EditorModeChanged>(m_editorMode, EditorMode::pawn));
        m_editorMode = EditorMode::pawn;
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

void MainForm::onSelectZoneNode(const nana::toolbar::item_proxy& drop_down_item, const std::string& node_name)
{

}

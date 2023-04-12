#include "MainForm.h"
#include "ViewerAppDelegate.h"
#include "SchemeColorDef.h"
#include "RenderPanel.h"
#include "OutputPanel.h"
#include "ModelInfoPanel.h"
#include "AnimationInfoPanel.h"
#include "Platforms/MemoryMacro.h"
#include "GraphicKernel/IGraphicAPI.h"
#include "nana/gui/filebox.hpp"
#include "DaeParser.h"

using namespace EnigmaViewer;
using namespace Enigma::Graphics;
using namespace std::chrono_literals;

MainForm::MainForm()
{
    m_menubar = nullptr;
    m_tabbar = nullptr;
    m_appDelegate = nullptr;
    m_timer = nullptr;
    m_renderPanel = nullptr;
    m_outputPanel = nullptr;
    m_modelInfoPanel = nullptr;
    m_animationInfoPanel = nullptr;
}

MainForm::~MainForm()
{
    SAFE_DELETE(m_tabbar);
    SAFE_DELETE(m_menubar);
    SAFE_DELETE(m_timer);
    SAFE_DELETE(m_appDelegate);
    SAFE_DELETE(m_renderPanel);
    SAFE_DELETE(m_outputPanel);
    SAFE_DELETE(m_modelInfoPanel);
    SAFE_DELETE(m_animationInfoPanel);
}

void MainForm::Initialize()
{
    UISchemeColors::ApplySchemaColors(scheme());
    get_place().div("vert<menubar weight=28>< <renderpanel weight=70%> | <vert<toolsbar weight=28> <toolsframe> > > <outputpanel weight=100>");
    InitMenu();

    m_renderPanel = menew RenderPanel{ *this };
    get_place().field("renderpanel") << *m_renderPanel;

    m_tabbar = menew nana::tabbar<int>{ *this };
    UISchemeColors::ApplySchemaColors(m_tabbar->scheme());
    get_place().field("toolsbar") << *m_tabbar;

    m_modelInfoPanel = menew ModelInfoPanel{ *this };
    m_modelInfoPanel->Initialize(this);
    get_place().field("toolsframe").fasten(*m_modelInfoPanel);
    m_tabbar->append("Model Info", *m_modelInfoPanel);

    m_animationInfoPanel = menew AnimationInfoPanel{ *this };
    m_animationInfoPanel->Initialize();
    get_place().field("toolsframe").fasten(*m_animationInfoPanel);
    m_tabbar->append("Animation Info", *m_animationInfoPanel);

    m_outputPanel = menew OutputPanel{ *this };
    m_outputPanel->Initialize();
    get_place()["outputpanel"] << *m_outputPanel;

    m_appDelegate = new ViewerAppDelegate();
    m_appDelegate->Initialize(IGraphicAPI::APIVersion::API_Dx11, IGraphicAPI::AsyncType::UseAsyncDevice, "viewer_log.log",
        reinterpret_cast<HWND>(m_renderPanel->native_handle()));
    m_timer = new nana::timer{ 1ms };
    m_timer->elapse([this] { m_appDelegate->OnTimerElapsed(); });
    m_timer->start();
    events().destroy([this] { this->Finalize(); });

    m_outputPanel->SubscribeHandlers();
    m_modelInfoPanel->SubscribeHandlers();

    get_place().collocate();
}

void MainForm::InitMenu()
{
    m_menubar = menew nana::menubar{ *this };
    m_menubar->scheme().background = UISchemeColors::BACKGROUND;
    m_menubar->scheme().body_selected = UISchemeColors::SELECT_BG;
    m_menubar->scheme().body_highlight = UISchemeColors::HIGHLIGHT_BG;
    m_menubar->scheme().text_fgcolor = UISchemeColors::FOREGROUND;
    m_menubar->push_back("&File");
    m_menubar->at(0).append("Import DAE File", [this](auto item) { OnImportDaeFile(item); });
    m_menubar->at(0).append("Save Animated Pawn", [this](auto item) { OnSaveAnimatedPawn(item); });
    m_menubar->at(0).append("Load Animated Pawn", [this](auto item) { OnLoadAnimatedPawn(item); });
    m_menubar->at(0).append_splitter();
    m_menubar->at(0).append("Exit", [this](auto item) { OnCloseCommand(item); });
    get_place().field("menubar") << *m_menubar;
}

void MainForm::OnImportDaeFile(const nana::menu::item_proxy& menu_item)
{
    nana::filebox fb{ *this, true };
    fb.add_filter({ {"Collada File(*.dae)", "*.dae"} });
    auto paths = fb.show();
    if (paths.size() > 0)
    {
        if (m_outputPanel)
        {
            m_outputPanel->AddMessage(std::string{ "open Collada file " } + paths[0].string());
        }
        DaeParser* parser = new DaeParser(Enigma::Controllers::GraphicMain::Instance()->GetSystemServiceAs<Enigma::Engine::GeometryRepository>());
        parser->LoadDaeFile(paths[0].string());
        m_appDelegate->LoadPawn(parser->GetPawnDto());
        m_modelInfoPanel->SetModelFileName(paths[0].stem().string());
        delete parser;
    }
}

void MainForm::OnSaveAnimatedPawn(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnLoadAnimatedPawn(const nana::menu::item_proxy& menu_item)
{

}

void MainForm::OnCloseCommand(const nana::menu::item_proxy& menu_item)
{
    close();
}

void MainForm::Finalize()
{
    if (m_outputPanel) m_outputPanel->UnsubscribeHandlers();
    if (m_modelInfoPanel) m_modelInfoPanel->UnsubscribeHandlers();
    if (m_appDelegate) m_appDelegate->Finalize();
}

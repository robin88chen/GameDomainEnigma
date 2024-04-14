#include "PawnListPanel.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "nana/gui/widgets/menu.hpp"
#include "Frameworks/CommandBus.h"
#include "ViewerCommands.h"

using namespace EnigmaViewer;

PawnListPanel::PawnListPanel(const nana::window& wd) : nana::panel<false>(wd)
{
    m_place = nullptr;
    m_pawnList = nullptr;
    m_popupMenu = nullptr;
}

PawnListPanel::~PawnListPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_pawnList);
    SAFE_DELETE(m_popupMenu);
}

void PawnListPanel::initialize(MainForm* main_form)
{
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<list_box>");
    m_pawnList = menew nana::listbox{ *this };
    UISchemeColors::ApplySchemaColors(m_pawnList->scheme());
    m_pawnList->scheme().header_bgcolor = UISchemeColors::BACKGROUND;
    m_pawnList->scheme().header_fgcolor = UISchemeColors::FOREGROUND;
    m_pawnList->scheme().item_highlighted = UISchemeColors::HIGHLIGHT_BG;
    m_pawnList->scheme().item_selected = UISchemeColors::SELECT_BG;
    m_pawnList->append_header("Name", 160);
    m_pawnList->events().mouse_down([this](const nana::arg_mouse& arg) { this->onPawnListMouseDown(arg); });
    m_place->field("list_box") << *m_pawnList;

    m_popupMenu = menew nana::menu{};
    m_popupMenu->append("Load Pawn", [this](auto item) { this->onLoadPawn(item); });
    m_popupMenu->append_splitter();
    m_popupMenu->append("Remove Pawn", [this](auto item) { this->onRemovePawn(item); });

    m_place->collocate();
}

void PawnListPanel::subscribeHandlers()
{
    m_refreshPawnList = std::make_shared<Enigma::Frameworks::CommandSubscriber>([=](const Enigma::Frameworks::ICommandPtr& c) { refreshPawnList(c); });
    Enigma::Frameworks::CommandBus::subscribe(typeid(RefreshPawnList), m_refreshPawnList);
}

void PawnListPanel::unsubscribeHandlers()
{
    Enigma::Frameworks::CommandBus::unsubscribe(typeid(RefreshPawnList), m_refreshPawnList);
    m_refreshPawnList = nullptr;
}

void PawnListPanel::refreshPawnList(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RefreshPawnList>(c);
    m_pawnList->clear();
    for (const auto& name : cmd->pawnList())
    {
        m_pawnList->at(0).append({ name });
    }
}

void PawnListPanel::onPawnListMouseDown(const nana::arg_mouse& arg)
{
    if (!arg.right_button) return;
    if (m_pawnList->selected().empty()) return;
    auto popuper = nana::menu_popuper(*m_popupMenu);
    popuper(arg);
}

void PawnListPanel::onLoadPawn(nana::menu::item_proxy& item)
{
    if (m_pawnList->selected().empty()) return;
    const auto selected = m_pawnList->selected().front();
    const auto name = m_pawnList->at(selected).text(0);
    Enigma::Frameworks::CommandBus::post(std::make_shared<LoadAnimatedPawn>(name));
}

void PawnListPanel::onRemovePawn(nana::menu::item_proxy& item)
{
    if (m_pawnList->selected().empty()) return;
    const auto selected = m_pawnList->selected().front();
    const auto name = m_pawnList->at(selected).text(0);
    Enigma::Frameworks::CommandBus::post(std::make_shared<RemoveAnimatedPawn>(name));
}

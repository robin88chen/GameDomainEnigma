#include "ModelListPanel.h"
#include "nana/gui/widgets/menu.hpp"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/CommandBus.h"
#include "ViewerCommands.h"

using namespace EnigmaViewer;

ModelListPanel::ModelListPanel(const nana::window& wd) : nana::panel<false>(wd)
{
    m_place = nullptr;
    m_modelList = nullptr;
    m_popupMenu = nullptr;
}

ModelListPanel::~ModelListPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_modelList);
    SAFE_DELETE(m_popupMenu);
}

void ModelListPanel::initialize(MainForm* main_form)
{
    m_place = menew nana::place{ *this };
    m_place->div("margin=[4,4,4,4] vert<list_box>");
    m_modelList = menew nana::listbox{ *this };
    UISchemeColors::ApplySchemaColors(m_modelList->scheme());
    m_modelList->scheme().header_bgcolor = UISchemeColors::BACKGROUND;
    m_modelList->scheme().header_fgcolor = UISchemeColors::FOREGROUND;
    m_modelList->scheme().item_highlighted = UISchemeColors::HIGHLIGHT_BG;
    m_modelList->scheme().item_selected = UISchemeColors::SELECT_BG;
    m_modelList->append_header("Name", 160);
    m_modelList->events().mouse_down([this](const nana::arg_mouse& arg) { this->onModelListMouseDown(arg); });
    m_place->field("list_box") << *m_modelList;

    m_popupMenu = menew nana::menu{};
    m_popupMenu->append("Load Model", [this](auto item) { this->onLoadModel(item); });
    m_place->collocate();
}

void ModelListPanel::subscribeHandlers()
{
    m_refreshModelList = std::make_shared<Enigma::Frameworks::CommandSubscriber>([=](const Enigma::Frameworks::ICommandPtr& c) { refreshModelList(c); });
    Enigma::Frameworks::CommandBus::subscribe(typeid(RefreshModelPrimitiveList), m_refreshModelList);
}

void ModelListPanel::unsubscribeHandlers()
{
    Enigma::Frameworks::CommandBus::unsubscribe(typeid(RefreshModelPrimitiveList), m_refreshModelList);
    m_refreshModelList = nullptr;
}

void ModelListPanel::refreshModelList(const Enigma::Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RefreshModelPrimitiveList>(c);
    m_modelList->clear();
    for (const auto& name : cmd->modelList())
    {
        m_modelList->at(0).append({ name });
    }
}

void ModelListPanel::onModelListMouseDown(const nana::arg_mouse& arg)
{
    if (!arg.right_button) return;
    if (m_modelList->selected().empty()) return;
    auto popuper = nana::menu_popuper(*m_popupMenu);
    popuper(arg);
}

void ModelListPanel::onLoadModel(nana::menu::item_proxy& item)
{
    if (m_modelList->selected().empty()) return;
    const auto selected = m_modelList->selected().front();
    const auto name = m_modelList->at(selected).text(0);
    Enigma::Frameworks::CommandBus::post(std::make_shared<LoadModelPrimitive>(name));
}


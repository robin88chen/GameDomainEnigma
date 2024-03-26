#include "ModelListPanel.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/CommandBus.h"
#include "ViewerCommands.h"

using namespace EnigmaViewer;

ModelListPanel::ModelListPanel(const nana::window& wd) : nana::panel<false>(wd)
{
    m_place = nullptr;
    m_modelList = nullptr;
}

ModelListPanel::~ModelListPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_modelList);
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
    m_place->field("list_box") << *m_modelList;

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


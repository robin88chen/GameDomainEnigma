#include "PawnListPanel.h"
#include "SchemeColorDef.h"
#include "Platforms/MemoryMacro.h"

using namespace EnigmaViewer;

PawnListPanel::PawnListPanel(const nana::window& wd) : nana::panel<false>(wd)
{
    m_place = nullptr;
    m_pawnList = nullptr;
}

PawnListPanel::~PawnListPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_pawnList);
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
    m_place->field("list_box") << *m_pawnList;

    m_place->collocate();
}


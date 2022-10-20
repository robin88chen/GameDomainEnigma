#include "SchemeColorDef.h"

nana::color s_bgBlack(45, 45, 48);
nana::color s_fgWhite(241, 241, 241);
nana::color s_bgSelect(32, 32, 32);
nana::color s_bgHighlight(62, 62, 64);
nana::color EnigmaViewer::UISchemeColors::BACKGROUND(s_bgBlack);
nana::color EnigmaViewer::UISchemeColors::FOREGROUND(s_fgWhite);
nana::color EnigmaViewer::UISchemeColors::SELECT_BG(s_bgSelect);
nana::color EnigmaViewer::UISchemeColors::HIGHLIGHT_BG(s_bgHighlight);

void EnigmaViewer::UISchemeColors::ApplySchemaColors(nana::widget_geometrics& scheme)
{
    scheme.background = UISchemeColors::BACKGROUND;
    scheme.foreground = UISchemeColors::FOREGROUND;
}

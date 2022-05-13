#include "SchemeColorDef.h"

nana::color s_bgBlack(45, 45, 48);
nana::color s_fgWhite(241, 241, 241);
nana::color s_bgSelect(32, 32, 32);
nana::color s_bgHighlight(62, 62, 64);
nana::color AssetPackageTool::UISchemeColors::BACKGROUND(s_bgBlack);
nana::color AssetPackageTool::UISchemeColors::FOREGROUND(s_fgWhite);
nana::color AssetPackageTool::UISchemeColors::SELECT_BG(s_bgSelect);
nana::color AssetPackageTool::UISchemeColors::HIGHLIGHT_BG(s_bgHighlight);

void AssetPackageTool::UISchemeColors::ApplySchemaColors(nana::widget_geometrics& scheme)
{
    scheme.background = UISchemeColors::BACKGROUND;
    scheme.foreground = UISchemeColors::FOREGROUND;
}

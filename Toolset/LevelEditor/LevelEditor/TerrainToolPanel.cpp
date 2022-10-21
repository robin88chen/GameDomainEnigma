#include "TerrainToolPanel.h"
#include "Platforms/MemoryMacro.h"
#include "SchemeColorDef.h"

using namespace LevelEditor;

constexpr int max_brush_size = 20;
constexpr unsigned int max_density_value = 20;

TerrainToolPanel::TerrainToolPanel(const nana::window& wd) : panel<false>{ wd }
{
    m_place = nullptr;
    m_toolbar = nullptr;
    m_terrainName = nullptr;
    m_brushSizeSpin = nullptr;
    m_brushSizeLabel = nullptr;
    m_brushHeight = nullptr;
    m_brushHeightLabel = nullptr;
    m_textureDensity = nullptr;
    m_textureDensityLabel = nullptr;
}

TerrainToolPanel::~TerrainToolPanel()
{
    SAFE_DELETE(m_place);
    SAFE_DELETE(m_toolbar);
    SAFE_DELETE(m_terrainName);
    SAFE_DELETE(m_brushSizeSpin);
    SAFE_DELETE(m_brushSizeLabel);
    SAFE_DELETE(m_brushHeight);
    SAFE_DELETE(m_brushHeightLabel);
    SAFE_DELETE(m_textureDensity);
    SAFE_DELETE(m_textureDensityLabel);
}

void TerrainToolPanel::Initialize(MainForm* form)
{
    m_mainForm = form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[2,2,2,2] vert<toolbar weight=28><terrain_name weight=28><brush_size weight=28><brush_height weight=28><texture_density weight=28><texture_btns weight=36 arrange=[32,32,32,32] margin=[2,2] gap=2>");

    m_toolbar = menew nana::toolbar(*this);
    UISchemeColors::ApplySchemaColors(m_toolbar->scheme());
    m_place->field("toolbar").fasten(*m_toolbar);

    m_terrainName = menew nana::label(*this, "Terrain Name");
    UISchemeColors::ApplySchemaColors(m_terrainName->scheme());
    m_place->field("terrain_name") << *m_terrainName;

    m_brushSizeLabel = menew nana::label(*this, "Brush Size");
    UISchemeColors::ApplySchemaColors(m_brushSizeLabel->scheme());
    m_brushSizeSpin = menew nana::spinbox(*this, true);
    UISchemeColors::ApplySchemaColors(m_brushSizeSpin->scheme());
    m_place->field("brush_size") << *m_brushSizeLabel << *m_brushSizeSpin;
    m_brushSizeSpin->range(1, max_brush_size, 1);
    m_brushSizeSpin->events().text_changed([this](const nana::arg_spinbox& a) { this->OnBrushSizeChanged(a); });

    m_brushHeightLabel = menew nana::label(*this, "Brush Height");
    UISchemeColors::ApplySchemaColors(m_brushHeightLabel->scheme());
    m_brushHeight = menew nana::textbox(*this, "0.3");
    UISchemeColors::ApplySchemaColors(m_brushHeight->scheme());
    m_brushHeight->events().text_changed([this](const nana::arg_textbox& a) { this->OnBrushHeightChanged(a); });
    m_place->field("brush_height") << *m_brushHeightLabel << *m_brushHeight;

    m_textureDensityLabel = menew nana::label(*this, "Texture Density");
    UISchemeColors::ApplySchemaColors(m_textureDensityLabel->scheme());
    m_textureDensity = menew nana::slider{ *this };
    UISchemeColors::ApplySchemaColors(m_textureDensity->scheme());
    m_textureDensity->maximum(max_density_value);
    m_textureDensity->events().value_changed([this](const nana::arg_slider& a) { this->OnLayerDensityChanged(a); });
    m_place->field("texture_density") << *m_textureDensityLabel << *m_textureDensity;

    m_place->collocate();
}

void TerrainToolPanel::SetTerrainName(const std::string& name)
{
    m_terrainName->caption(name);
}

void TerrainToolPanel::OnBrushSizeChanged(const nana::arg_spinbox& arg)
{
}

void TerrainToolPanel::OnBrushHeightChanged(const nana::arg_textbox& arg)
{
}

void TerrainToolPanel::OnLayerDensityChanged(const nana::arg_slider& arg)
{
}

void TerrainToolPanel::OnTextureLayerButton(const nana::arg_click& arg, unsigned int index)
{
}


#include "TerrainToolPanel.h"
#include "EditorUtilities.h"
#include "Platforms/MemoryMacro.h"
#include "SchemeColorDef.h"
#include "LevelEditorUiEvents.h"
#include "LevelEditorEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/StringFormat.h"
#include "Terrain/TerrainPawn.h"
#include "Terrain/TerrainPrimitive.h"
#include "FileSystem/FileSystem.h"
#include "GameEngine/Texture.h"
#include "TerrainEditService.h"

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
    for (auto& btn : m_textureLayerButtons)
    {
        SAFE_DELETE(btn);
    }
}

void TerrainToolPanel::initialize(MainForm* form, unsigned texture_btn_count)
{
    m_mainForm = form;
    m_place = menew nana::place{ *this };
    m_place->div("margin=[2,2,2,2] vert<toolbar weight=28><terrain_name weight=28><brush_size weight=28><brush_height weight=28><texture_density weight=28><texture_btns weight=36 arrange=[32,32,32,32] margin=[2,2] gap=2>");

    m_toolbar = menew nana::toolbar(*this);
    UISchemeColors::applySchemaColors(m_toolbar->scheme());
    m_toolbar->append(nana::toolbar::tools::toggle, "Raise Terrain Height",
        nana::paint::image("icons/raise_height.bmp")).toggle_group("brush_type")
        .answerer([this](const nana::toolbar::item_proxy& it)
            {
                this->onTerrainToolButton(it, TerrainEditToolSelected::Tool::Raise);
            });
    m_toolbar->append(nana::toolbar::tools::toggle, "Lower Terrain Height",
        nana::paint::image("icons/lower_height.bmp")).toggle_group("brush_type")
        .answerer([this](const nana::toolbar::item_proxy& it)
            {
                this->onTerrainToolButton(it, TerrainEditToolSelected::Tool::Lower);
            });
    m_toolbar->append(nana::toolbar::tools::toggle, "Paint Texture Layer",
        nana::paint::image("icons/texture_brush.bmp")).toggle_group("brush_type")
        .answerer([this](const nana::toolbar::item_proxy& it)
            {
                this->onTerrainToolButton(it, TerrainEditToolSelected::Tool::Paint);
            });
    m_place->field("toolbar").fasten(*m_toolbar);

    m_terrainName = menew nana::label(*this, "Terrain Name");
    UISchemeColors::applySchemaColors(m_terrainName->scheme());
    m_place->field("terrain_name") << *m_terrainName;

    m_brushSizeLabel = menew nana::label(*this, "Brush Size");
    UISchemeColors::applySchemaColors(m_brushSizeLabel->scheme());
    m_brushSizeSpin = menew nana::spinbox(*this, true);
    UISchemeColors::applySchemaColors(m_brushSizeSpin->scheme());
    m_place->field("brush_size") << *m_brushSizeLabel << *m_brushSizeSpin;
    m_brushSizeSpin->range(1, max_brush_size, 1);
    m_brushSizeSpin->events().text_changed([this](const nana::arg_spinbox& a) { this->onBrushSizeChanged(a); });

    m_brushHeightLabel = menew nana::label(*this, "Brush Height");
    UISchemeColors::applySchemaColors(m_brushHeightLabel->scheme());
    m_brushHeight = menew nana::textbox(*this, "0.3");
    UISchemeColors::applySchemaColors(m_brushHeight->scheme());
    m_brushHeight->events().text_changed([this](const nana::arg_textbox& a) { this->onBrushHeightChanged(a); });
    m_place->field("brush_height") << *m_brushHeightLabel << *m_brushHeight;

    m_textureDensityLabel = menew nana::label(*this, "Texture Density");
    UISchemeColors::applySchemaColors(m_textureDensityLabel->scheme());
    m_textureDensity = menew nana::slider{ *this };
    UISchemeColors::applySchemaColors(m_textureDensity->scheme());
    m_textureDensity->maximum(max_density_value);
    m_textureDensity->vernier([this](unsigned int maximum, unsigned int value)
        { return string_format("%6.2f", slideValueToDensity(value)); });
    m_textureDensity->value(10);
    m_textureDensity->events().value_changed([this](const nana::arg_slider& a) { this->onLayerDensityChanged(a); });
    m_place->field("texture_density") << *m_textureDensityLabel << *m_textureDensity;

    m_textureLayerButtons.resize(texture_btn_count);
    for (unsigned i = 0; i < texture_btn_count; i++)
    {
        m_textureLayerButtons[i] = menew nana::button(*this, nana::rectangle(nana::size(64, 64)));
        m_textureLayerButtons[i]->enable_pushed(true);
        m_textureLayerButtons[i]->events().click([=](const nana::arg_click& a) { this->onTextureLayerButton(a, i); });
        m_place->field("texture_btns") << *m_textureLayerButtons[i];
        if (i == 0)
        {
            m_textureLayerButtons[i]->focus();
        }
    }

    m_place->collocate();
}

void TerrainToolPanel::subscribeHandlers()
{
    m_onPickedSpatialChanged = std::make_shared<Enigma::Frameworks::EventSubscriber>([=](auto e) { onPickedSpatialChanged(e); });
    Enigma::Frameworks::EventPublisher::subscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);
}

void TerrainToolPanel::unsubscribeHandlers()
{
    Enigma::Frameworks::EventPublisher::unsubscribe(typeid(PickedSpatialChanged), m_onPickedSpatialChanged);
    m_onPickedSpatialChanged = nullptr;
}

void TerrainToolPanel::setTerrainName(const std::string& name)
{
    m_terrainName->caption(name);
}

void TerrainToolPanel::onBrushSizeChanged(const nana::arg_spinbox& arg)
{
    if (!m_brushSizeSpin) return;
    Enigma::Frameworks::EventPublisher::post(std::make_shared<TerrainBrushSizeChanged>(m_brushSizeSpin->to_int()));
}

void TerrainToolPanel::onBrushHeightChanged(const nana::arg_textbox& arg)
{
    if ((m_brushHeight) && (!m_brushHeight->text().empty()))
    {
        auto height = static_cast<float>(m_brushHeight->to_double());
        Enigma::Frameworks::EventPublisher::post(std::make_shared<TerrainBrushHeightChanged>(height));
    }
}

void TerrainToolPanel::onLayerDensityChanged(const nana::arg_slider& arg)
{
    if (!m_textureDensity) return;
    auto density = slideValueToDensity(m_textureDensity->value());
    Enigma::Frameworks::EventPublisher::post(std::make_shared<TerrainBrushDensityChanged>(density));
}

void TerrainToolPanel::onTextureLayerButton(const nana::arg_click& arg, unsigned int index)
{
    if (index >= m_textureLayerButtons.size()) return;
    Enigma::Frameworks::EventPublisher::post(std::make_shared<TerrainPaintingLayerChanged>(index));
}

void TerrainToolPanel::onTerrainToolButton(const nana::toolbar::item_proxy& it, TerrainEditToolSelected::Tool tool)
{
    Enigma::Frameworks::EventPublisher::post(std::make_shared<TerrainEditToolSelected>(tool));
}

unsigned int TerrainToolPanel::densityToSlideValue(float density) const
{
    int v = static_cast<int>(density * 10.0f + 10.0f);
    return (unsigned int)std::clamp(v, 0, static_cast<int>(max_density_value));
}

float TerrainToolPanel::slideValueToDensity(unsigned int value) const
{
    return (static_cast<float>(value) - 10.0f) / 10.0f;
}

void TerrainToolPanel::refreshTextureLayerButtons(const Enigma::Engine::EffectTextureMap& texture_map)
{
    for (unsigned int i = 0; i < TerrainEditService::LayerSemantics.size(); i++)
    {
        auto semantic_tex = texture_map.findSemanticTexture(TerrainEditService::LayerSemantics[i]);
        if (!semantic_tex) continue;
        auto tex = std::get<std::shared_ptr<Enigma::Engine::Texture>>(*semantic_tex);
        if (!tex) continue;
        Enigma::FileSystem::Filename filename(tex->factoryDesc().GetResourceFilename());
        std::string filepath = Enigma::FileSystem::FileSystem::instance()->getStdioFullPath(filename.getSubPathFileName(), filename.getMountPathId());
        pasteTextureImageToButton(filepath, m_textureLayerButtons[i], 64);
        m_textureLayerButtons[i]->focus();
    }
}

void TerrainToolPanel::onPickedSpatialChanged(const Enigma::Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<PickedSpatialChanged>(e);
    if (!ev) return;
    auto terrain = std::dynamic_pointer_cast<Enigma::Terrain::TerrainPawn>(ev->spatial());
    if (!terrain) return;
    auto terrain_prim = std::dynamic_pointer_cast<Enigma::Terrain::TerrainPrimitive>(terrain->getPrimitive());
    if (!terrain_prim) return;
    setTerrainName(terrain->id().name());
    refreshTextureLayerButtons(terrain_prim->getTextureMap(0));
}

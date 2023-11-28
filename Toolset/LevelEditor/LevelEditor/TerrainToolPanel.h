/*********************************************************************
 * \file   TerrainToolPanel.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef TERRAIN_TOOL_PANEL_H
#define TERRAIN_TOOL_PANEL_H

#include "Frameworks/EventSubscriber.h"
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/toolbar.hpp>
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/button.hpp>
#include "LevelEditorUiEvents.h"
#include "GameEngine/EffectTextureMap.h"

namespace LevelEditor
{
    class MainForm;
    class TerrainToolPanel : public nana::panel<false>
    {
    public:
        TerrainToolPanel(const nana::window& wd);
        virtual ~TerrainToolPanel();

        void initialize(MainForm* form, unsigned texture_btn_count);
        void setTerrainName(const std::string& name);

        void subscribeHandlers();
        void unsubscribeHandlers();

    private:
        void onBrushSizeChanged(const nana::arg_spinbox& arg);
        void onBrushHeightChanged(const nana::arg_textbox& arg);
        void onLayerDensityChanged(const nana::arg_slider& arg);
        void onTextureLayerButton(const nana::arg_click& arg, unsigned int index);
        void onTerrainToolButton(const nana::toolbar::item_proxy& it, TerrainEditToolSelected::Tool tool);

        unsigned int densityToSlideValue(float density) const;
        float slideValueToDensity(unsigned int value) const;

        void refreshTextureLayerButtons(const Enigma::Engine::EffectTextureMap& texture_map);

        void onPickedSpatialChanged(const Enigma::Frameworks::IEventPtr& e);

    private:
        MainForm* m_mainForm;
        nana::place* m_place;
        nana::toolbar* m_toolbar;
        nana::label* m_terrainName;
        nana::spinbox* m_brushSizeSpin;
        nana::label* m_brushSizeLabel;
        nana::textbox* m_brushHeight;
        nana::label* m_brushHeightLabel;
        nana::slider* m_textureDensity;
        nana::label* m_textureDensityLabel;
        std::vector<nana::button*> m_textureLayerButtons;
        Enigma::Frameworks::EventSubscriberPtr m_onPickedSpatialChanged;
    };
}


#endif // TERRAIN_TOOL_PANEL_H

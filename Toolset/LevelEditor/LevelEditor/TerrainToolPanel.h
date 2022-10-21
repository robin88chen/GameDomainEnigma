/*********************************************************************
 * \file   TerrainToolPanel.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef TERRAIN_TOOL_PANEL_H
#define TERRAIN_TOOL_PANEL_H

#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/widgets/toolbar.hpp>
#include <nana/gui/widgets/spinbox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/slider.hpp>

namespace LevelEditor
{
    class MainForm;
    class TerrainToolPanel : public nana::panel<false>
    {
    public:
        TerrainToolPanel(const nana::window& wd);
        virtual ~TerrainToolPanel();

        void Initialize(MainForm* form);
        void SetTerrainName(const std::string& name);

    private:
        void OnBrushSizeChanged(const nana::arg_spinbox& arg);
        void OnBrushHeightChanged(const nana::arg_textbox& arg);
        void OnLayerDensityChanged(const nana::arg_slider& arg);
        void OnTextureLayerButton(const nana::arg_click& arg, unsigned int index);

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
    };
}


#endif // TERRAIN_TOOL_PANEL_H

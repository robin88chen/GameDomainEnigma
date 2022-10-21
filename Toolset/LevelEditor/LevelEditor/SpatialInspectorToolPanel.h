/*********************************************************************
 * \file   SpatialInspectorToolPanel.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _SPATIAL_INSPECTOR_TOOL_PANEL_H
#define _SPATIAL_INSPECTOR_TOOL_PANEL_H

#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/place.hpp"
#include "propertygrid/propertygrid.h"

namespace LevelEditor
{
    class MainForm;
    class SpatialInspectorPanel : public nana::panel<false>
    {
    public:
        SpatialInspectorPanel(const nana::window& wd);
        virtual ~SpatialInspectorPanel();

        void Initialize(MainForm* form);
        void Finalize();

    private:
        void OnPropertyChanged(const nana::arg_propertygrid& arg);
        void OnAttributePropertiesChanged(size_t index, const std::string& value);
        void OnLocalSpatialPropertiesChanged(size_t index, const std::string& value);
        void OnWorldSpatialPropertiesChanged(size_t index, const std::string& value);
    private:
        MainForm* m_mainForm;
        nana::propertygrid* m_properties;
        nana::place* m_place;
    };
}


#endif // _SPATIAL_INSPECTOR_TOOL_PANEL_H

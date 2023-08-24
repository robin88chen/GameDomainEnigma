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
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/Spatial.h"
#include "SceneGraph/Light.h"

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

        void SubscribeHandlers();
        void UnsubscribeHandlers();

    private:
        void OnPropertyChanged(const nana::arg_propertygrid& arg);
        void OnLightPropertyChanged(const nana::arg_propertygrid& arg);
        void OnAttributePropertiesChanged(size_t index, const std::string& value);
        void OnLocalSpatialPropertiesChanged(size_t index, const std::string& value);
        void OnWorldSpatialPropertiesChanged(size_t index, const std::string& value);

        void ShowSpatialProperties(const std::shared_ptr<Enigma::SceneGraph::Spatial>& spatial);
        void ShowLightProperties(const std::shared_ptr<Enigma::SceneGraph::Light>& light);

        void OnPickedSpatialChanged(const Enigma::Frameworks::IEventPtr& e);
    private:
        MainForm* m_mainForm;
        nana::propertygrid* m_properties;
        nana::propertygrid* m_lightProperties;
        nana::place* m_place;

        Enigma::Frameworks::EventSubscriberPtr m_onPickedSpatialChanged;

        std::weak_ptr<Enigma::SceneGraph::Spatial> m_selectedSpatial;
    };
}


#endif // _SPATIAL_INSPECTOR_TOOL_PANEL_H

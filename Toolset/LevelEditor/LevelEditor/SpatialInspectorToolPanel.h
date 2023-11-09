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

        void initialize(MainForm* form);
        void finalize();

        void subscribeHandlers();
        void unsubscribeHandlers();

    private:
        void onPropertyChanged(const nana::arg_propertygrid& arg);
        void onLightPropertyChanged(const nana::arg_propertygrid& arg);
        void onAttributePropertiesChanged(size_t index, const std::string& value);
        void onLocalSpatialPropertiesChanged(size_t index, const std::string& value);
        void onWorldSpatialPropertiesChanged(size_t index, const std::string& value);

        void showSpatialProperties(const std::shared_ptr<Enigma::SceneGraph::Spatial>& spatial);
        void showLightProperties(const std::shared_ptr<Enigma::SceneGraph::Light>& light);

        void onPickedSpatialChanged(const Enigma::Frameworks::IEventPtr& e);
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

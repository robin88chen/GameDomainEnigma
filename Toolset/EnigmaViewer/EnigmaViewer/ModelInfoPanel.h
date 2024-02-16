/*********************************************************************
 * \file   ModelInfoPanel.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _MODEL_INFO_PANEL_H
#define _MODEL_INFO_PANEL_H

#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/place.hpp"
#include "nana/gui/widgets/label.hpp"
#include "nana/gui/widgets/treebox.hpp"
#include "nana/gui/widgets/menu.hpp"
#include "Renderables/ModelPrimitive.h"
#include "Frameworks/CommandSubscriber.h"

namespace EnigmaViewer
{
    class MainForm;

    class ModelInfoPanel : public nana::panel<false>
    {
    public:
        ModelInfoPanel(const nana::window& wd);
        virtual ~ModelInfoPanel();

        void initialize(MainForm* main_form);

        void setModelFileName(const std::string& filename);
        void enumModelMeshNode(const std::shared_ptr<Enigma::Renderables::ModelPrimitive>& model);
        void onMeshNodeTreeMouseDown(const nana::arg_mouse& arg);
        void onAddCloudParticle(const nana::menu::item_proxy& menu_item);
        void onAddSuperSprayParticle(const nana::menu::item_proxy& menu_item);
        void onChangeMeshTexture(const nana::menu::item_proxy& menu_item);

        void subscribeHandlers();
        void unsubscribeHandlers();

    private:
        void refreshModelNodeTree(const Enigma::Frameworks::ICommandPtr& c);

    private:
        MainForm* m_main;
        nana::place* m_place;
        nana::label* m_modelNamePrompt;
        nana::label* m_modelNameLabel;
        nana::treebox* m_meshNodeTree;
        nana::menu* m_popupMenu;

        Enigma::Frameworks::CommandSubscriberPtr m_refreshModelNodeTree;
    };
}


#endif // _MODEL_INFO_PANEL_H

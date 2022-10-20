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

namespace EnigmaViewer
{
    class MainForm;

    class ModelInfoPanel : public nana::panel<false>
    {
    public:
        ModelInfoPanel(const nana::window& wd);
        virtual ~ModelInfoPanel();

        void Initialize(MainForm* main_form);

        void SetModelFileName(const std::string& filename);
        //void EnumModelMeshNode(const Enigma::ModelPrimitivePtr& model);
        void OnMeshNodeTreeMouseDown(const nana::arg_mouse& arg);
        void OnAddCloudParticle(const nana::menu::item_proxy& menu_item);
        void OnAddSuperSprayParticle(const nana::menu::item_proxy& menu_item);
        void OnChangeMeshTexture(const nana::menu::item_proxy& menu_item);

    private:
        MainForm* m_main;
        nana::place* m_place;
        nana::label* m_modelNamePrompt;
        nana::label* m_modelNameLabel;
        nana::treebox* m_meshNodeTree;
        nana::menu* m_popupMenu;
    };
}


#endif // _MODEL_INFO_PANEL_H

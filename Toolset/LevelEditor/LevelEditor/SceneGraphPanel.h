/*********************************************************************
 * \file   SceneGraphPanel.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _SCENE_GRAPH_PANEL_H
#define _SCENE_GRAPH_PANEL_H

#include <nana/gui/place.hpp>
#include "nana/gui/widgets/panel.hpp"
#include "nana/gui/widgets/treebox.hpp"

namespace LevelEditor
{
    class MainForm;

    class SceneGraphPanel : public nana::panel<false>
    {
    public:
        SceneGraphPanel(const nana::window& wd);
        virtual ~SceneGraphPanel();

        void Initialize(MainForm* main_form);
        void Finalize();

        void RefreshSceneGraphTree();

    private:
        void OnSceneGraphTreeSelected(const nana::arg_treebox& arg);

    private:
        MainForm* m_main;
        nana::place* m_place;
        nana::treebox* m_sceneGraphTree;
    };
}

#endif // _SCENE_GRAPH_PANEL_H

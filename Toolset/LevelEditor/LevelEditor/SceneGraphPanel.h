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
#include "SceneGraph/Spatial.h"
#include "Frameworks/CommandSubscriber.h"

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

        void RefreshSceneGraphTree(const std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>>& flattened_scene);

        void SubscribeHandlers();
        void UnsubscribeHandlers();

    private:
        void OnSceneGraphTreeSelected(const nana::arg_treebox& arg);

        void DoRefreshingSceneGraphTree(const Enigma::Frameworks::ICommandPtr& c);
    private:
        MainForm* m_main;
        nana::place* m_place;
        nana::treebox* m_sceneGraphTree;

        Enigma::Frameworks::CommandSubscriberPtr m_doRefreshingSceneGraph;
    };
}

#endif // _SCENE_GRAPH_PANEL_H

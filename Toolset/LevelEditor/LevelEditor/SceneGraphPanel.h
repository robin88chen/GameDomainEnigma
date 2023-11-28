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

        void initialize(MainForm* main_form);
        void finalize();

        void refreshSceneGraphTree(const std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>>& flattened_scene);

        void subscribeHandlers();
        void unsubscribeHandlers();

    private:
        void onSceneGraphTreeSelected(const nana::arg_treebox& arg);

        void refreshSceneGraphTree(const Enigma::Frameworks::ICommandPtr& c);
    private:
        MainForm* m_main;
        nana::place* m_place;
        nana::treebox* m_sceneGraphTree;

        Enigma::Frameworks::CommandSubscriberPtr m_refreshSceneGraph;
    };
}

#endif // _SCENE_GRAPH_PANEL_H

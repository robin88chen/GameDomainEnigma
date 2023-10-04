/*********************************************************************
 * \file   MainForm.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _MAIN_FORM_H
#define _MAIN_FORM_H

#include "nana/gui.hpp"
#include "nana/gui/widgets/menubar.hpp"
#include "nana/gui/widgets/tabbar.hpp"
#include "nana/gui/timer.hpp"
#include "nana/gui/widgets/toolbar.hpp"
#include "nana/gui/widgets/label.hpp"
#include "EditorModes.h"

namespace LevelEditor
{
    class EditorAppDelegate;
    class RenderPanel;
    class SceneGraphPanel;
    class SpatialInspectorPanel;
    class TerrainToolPanel;
    class OutputPanel;
    class WorldEditConsole;
    class PawnEditConsole;

    class MainForm : public nana::form
    {
    public:
        enum class ToolIndex : size_t
        {
            ToolCursor,
            ToolTerrain,
            ToolEntity,
            ToolSeparator1,
            ToolMoveEntity,
            ToolRotateEntity,
            ToolScaleEntity,
            ToolSeparator2,
            ToolCandidateEntity,
            ToolSeparator3,
            ToolGodMode,
            ToolSeparator4,
            ToolZoneSelect,
        };
    public:
        MainForm();
        virtual ~MainForm();

        void InitSubPanels();

        void InitializeGraphics();

        SceneGraphPanel* GetSceneGraphPanel() { return m_sceneGraphPanel; }
        RenderPanel* GetRenderPanel() { return m_renderPanel; }
        SpatialInspectorPanel* GetSpatialInspectorPanel() { return m_spatialInspectorPanel; }

        inline static std::string DataKey = "LevelEditor::MainForm";
    private:
        void InitMenu();
        void InitPanels();
        void InitTools();

        void OnCloseCommand(const nana::menu::item_proxy& menu_item);
        void OnCreateWorldMapCommand(const nana::menu::item_proxy& menu_item);
        void OnLoadWorldCommand(const nana::menu::item_proxy& menu_item);
        void OnSaveWorldCommand(const nana::menu::item_proxy& menu_item);
        void OnAddTerrainCommand(const nana::menu::item_proxy& menu_item);
        void OnAddEnvironmentLightCommand(const nana::menu::item_proxy& menu_item);

        void OnCreateZoneNodeCommand(const nana::menu::item_proxy& menu_item);
        void OnAddPortalCommand(const nana::menu::item_proxy& menu_item);

        void OnToolBarSelected(const nana::arg_toolbar& arg);

        void OnAddCandidatePawn(const nana::toolbar::item_proxy& drop_down_item);
        void OnSelectPawn(const nana::toolbar::item_proxy& drop_down_item, const std::string& pawn_name);
        void OnSelectZoneNode(const nana::toolbar::item_proxy& drop_down_item, const std::string& node_name);
        /*void OnImportDaeFile(const nana::menu::item_proxy& menu_item);
        void OnSaveEntity(const nana::menu::item_proxy& menu_item);
        void OnLoadEntity(const nana::menu::item_proxy& menu_item);*/

        void OnCameraFrustumCommand(const nana::menu::item_proxy& menu_item);

        void OnGodModeChanged(bool enabled);
        void FinalizeGraphics();

    private:
        nana::menubar* m_menubar;
        nana::tabbar<int>* m_tabbar;
        nana::toolbar* m_toolbar;
        nana::label* m_statusCameraZone;
        EditorAppDelegate* m_appDelegate;
        nana::timer* m_timer;
        RenderPanel* m_renderPanel;
        SceneGraphPanel* m_sceneGraphPanel;
        SpatialInspectorPanel* m_spatialInspectorPanel;
        TerrainToolPanel* m_terrainToolPanel;
        OutputPanel* m_outputPanel;

        std::weak_ptr<WorldEditConsole> m_worldConsole;
        std::weak_ptr<PawnEditConsole> m_pawnConsole;

        EditorMode m_editorMode;
    };
}


#endif // _MAIN_FORM_H

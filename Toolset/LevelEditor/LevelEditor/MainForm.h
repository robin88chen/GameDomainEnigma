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
#include "nana/gui/widgets/treebox.hpp"
#include "nana/gui/dragdrop.hpp"
#include "EditorModes.h"

namespace LevelEditor
{
    class EditorAppDelegate;
    class RenderPanel;
    class SceneGraphPanel;
    class SpatialInspectorPanel;
    class TerrainToolPanel;
    class OutputPanel;
    class AssetsBrowsePanel;
    class WorldEditConsole;
    class PawnEditConsole;
    class TerrainEditConsole;
    class EditorSceneConsole;

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

        void initSubPanels();

        void initializeGraphics();

        SceneGraphPanel* getSceneGraphPanel() { return m_sceneGraphPanel; }
        RenderPanel* getRenderPanel() { return m_renderPanel; }
        SpatialInspectorPanel* getSpatialInspectorPanel() { return m_spatialInspectorPanel; }

        void bindAssetToSceneGraphDragdrop();

        inline static std::string DataKey = "LevelEditor::MainForm";
    private:
        void initMenu();
        void initPanels();
        void initTools();

        void onCloseCommand(const nana::menu::item_proxy& menu_item);
        void onCreateWorldMapCommand(const nana::menu::item_proxy& menu_item);
        void onLoadWorldCommand(const nana::menu::item_proxy& menu_item);
        void onSaveWorldCommand(const nana::menu::item_proxy& menu_item);
        void onAddTerrainCommand(const nana::menu::item_proxy& menu_item);
        void onAddAmbientLightCommand(const nana::menu::item_proxy& menu_item);
        void onAddSunLightCommand(const nana::menu::item_proxy& menu_item);
        void onAddPointLightCommand(const nana::menu::item_proxy& menu_item);

        void onCreateZoneNodeCommand(const nana::menu::item_proxy& menu_item);
        void onAddPortalCommand(const nana::menu::item_proxy& menu_item);

        void onToolBarSelected(const nana::arg_toolbar& arg);

        void onAddCandidatePawn(const nana::toolbar::item_proxy& drop_down_item);
        void onSelectPawn(const nana::toolbar::item_proxy& drop_down_item, const std::string& pawn_name);
        void onSelectZoneNode(const nana::toolbar::item_proxy& drop_down_item, const std::string& node_name);
        /*void OnImportDaeFile(const nana::menu::item_proxy& menu_item);
        void OnSaveEntity(const nana::menu::item_proxy& menu_item);
        void OnLoadEntity(const nana::menu::item_proxy& menu_item);*/

        void onCameraFrustumCommand(const nana::menu::item_proxy& menu_item);

        void onGodModeChanged(bool enabled);
        void finalizeGraphics();

        nana::treebox* getAssetsTree() const;
        bool isAssetHovered() const;
        nana::treebox* getSceneGraphTree() const;
        void dropAssetToScene();
        bool hasDropTargetNodeSelected() const;

    private:
        nana::menubar* m_menubar;
        nana::tabbar<int>* m_tabbar;
        nana::toolbar* m_toolbar;
        nana::label* m_statusCameraZone;
        EditorAppDelegate* m_appDelegate;
        nana::timer* m_timer;
        RenderPanel* m_renderPanel;
        SceneGraphPanel* m_sceneGraphPanel;
        AssetsBrowsePanel* m_assetsBrowsePanel;
        SpatialInspectorPanel* m_spatialInspectorPanel;
        TerrainToolPanel* m_terrainToolPanel;
        OutputPanel* m_outputPanel;

        nana::simple_dragdrop* m_assetToSceneGraphDragDrop;

        std::weak_ptr<WorldEditConsole> m_worldConsole;
        std::weak_ptr<TerrainEditConsole> m_terrainConsole;
        std::weak_ptr<PawnEditConsole> m_pawnConsole;
        std::weak_ptr<EditorSceneConsole> m_sceneConsole;

        EditorMode m_editorMode;
    };
}


#endif // _MAIN_FORM_H

/*********************************************************************
 * \file   TerrainEditConsole.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef TERRAIN_EDIT_CONSOLE_H
#define TERRAIN_EDIT_CONSOLE_H

#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/Pawn.h"
#include "GraphicKernel/TargetViewPort.h"

namespace LevelEditor
{
    class GeometryDataFileStoreMapper;

    class TerrainEditConsole : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        enum class TerrainEditMode
        {
            unknown = 0,
            raiseHeight,
            lowerHeight,
            paintTexture,
        };
    public:
        TerrainEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<GeometryDataFileStoreMapper>& geometry_data_file_store_mapper, const std::string& terrain_path, const std::string& media_path_id);
        TerrainEditConsole(const TerrainEditConsole&) = delete;
        TerrainEditConsole& operator=(const TerrainEditConsole&) = delete;
        virtual ~TerrainEditConsole() override;
        TerrainEditConsole(TerrainEditConsole&&) = delete;
        TerrainEditConsole& operator=(TerrainEditConsole&&) = delete;

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

        bool isTerrainNameDuplicated(const std::string& terrain_name) const;

        const std::string& terrainPath() const { return m_terrainPath; }
        const std::string& mediaPathId() const { return m_mediaPathId; }

    protected:
        //void DoCreatingNewTerrain(const Enigma::Frameworks::ICommandPtr& c);
        void onEditorModeChanged(const Enigma::Frameworks::IEventPtr& e);
        void onSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
        void onPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
        void onTerrainBrushSizeChanged(const Enigma::Frameworks::IEventPtr& e);
        void onTerrainBrushHeightChanged(const Enigma::Frameworks::IEventPtr& e);
        void onTerrainBrushDensityChanged(const Enigma::Frameworks::IEventPtr& e);
        void onTerrainPaintingLayerChanged(const Enigma::Frameworks::IEventPtr& e);
        void onTerrainToolSelected(const Enigma::Frameworks::IEventPtr& e);
        void onSceneCursorMoved(const Enigma::Frameworks::IEventPtr& e);
        void onSceneCursorDragged(const Enigma::Frameworks::IEventPtr& e);
        void onSceneCursorPressed(const Enigma::Frameworks::IEventPtr& e);
        void onSceneCursorReleased(const Enigma::Frameworks::IEventPtr& e);

        void createBrushPawn();
        void sendTerrainEditCommand(float elapse_time);

    protected:
        bool m_isEnabled;
        std::weak_ptr<GeometryDataFileStoreMapper> m_geometryDataFileStoreMapper;
        std::string m_terrainPath;
        std::string m_mediaPathId;
        std::weak_ptr<Enigma::SceneGraph::Pawn> m_brush;

        //Enigma::Frameworks::CommandSubscriberPtr m_doCreatingNewTerrain;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorMoved;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorDragged;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorPressed;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorReleased;
        Enigma::Frameworks::EventSubscriberPtr m_onEditorModeChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
        Enigma::Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainBrushSizeChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainBrushHeightChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainBrushDensityChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainPaintingLayerChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainToolSelected;

        TerrainEditMode m_currMode;
        unsigned m_brushSize;
        float m_brushHeight;
        float m_brushDensity;
        unsigned m_layerIndex;
    };
}

#endif // TERRAIN_EDIT_CONSOLE_H

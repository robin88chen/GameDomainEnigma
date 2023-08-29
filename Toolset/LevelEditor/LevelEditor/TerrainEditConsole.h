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
    class TerrainEditConsole : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        enum class TerrainEditMode
        {
            Mode_Unknown = 0,
            Mode_RaiseHeight,
            Mode_LowerHeight,
            Mode_PaintTexture,
        };
    public:
        TerrainEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr);
        TerrainEditConsole(const TerrainEditConsole&) = delete;
        TerrainEditConsole& operator=(const TerrainEditConsole&) = delete;
        virtual ~TerrainEditConsole() override;
        TerrainEditConsole(TerrainEditConsole&&) = delete;
        TerrainEditConsole& operator=(TerrainEditConsole&&) = delete;

        virtual Enigma::Frameworks::ServiceResult OnInit() override;
        virtual Enigma::Frameworks::ServiceResult OnTerm() override;

    protected:
        //void DoCreatingNewTerrain(const Enigma::Frameworks::ICommandPtr& c);
        void OnEditorModeChanged(const Enigma::Frameworks::IEventPtr& e);
        void OnSceneGraphBuilt(const Enigma::Frameworks::IEventPtr& e);
        void OnPawnPrimitiveBuilt(const Enigma::Frameworks::IEventPtr& e);
        void OnTerrainBrushSizeChanged(const Enigma::Frameworks::IEventPtr& e);
        void OnTerrainBrushHeightChanged(const Enigma::Frameworks::IEventPtr& e);
        void OnTerrainToolSelected(const Enigma::Frameworks::IEventPtr& e);
        void OnSceneCursorMoved(const Enigma::Frameworks::IEventPtr& e);

        void CreateBrushPawn();

    protected:
        bool m_isEnabled;
        std::weak_ptr<Enigma::SceneGraph::Pawn> m_brush;

        //Enigma::Frameworks::CommandSubscriberPtr m_doCreatingNewTerrain;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorMoved;
        Enigma::Frameworks::EventSubscriberPtr m_onEditorModeChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
        Enigma::Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainBrushSizeChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainBrushHeightChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onTerrainToolSelected;

        TerrainEditMode m_currMode;
        unsigned m_brushSize;
        float m_brushHeight;
    };
}

#endif // TERRAIN_EDIT_CONSOLE_H

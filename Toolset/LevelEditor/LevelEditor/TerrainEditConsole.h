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

namespace LevelEditor
{
    class TerrainEditConsole : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
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

        void CreateBrushPawn();

    protected:
        bool m_isEnabled;
        std::weak_ptr<Enigma::SceneGraph::Pawn> m_brush;

        //Enigma::Frameworks::CommandSubscriberPtr m_doCreatingNewTerrain;
        Enigma::Frameworks::EventSubscriberPtr m_onEditorModeChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
    };
}

#endif // TERRAIN_EDIT_CONSOLE_H

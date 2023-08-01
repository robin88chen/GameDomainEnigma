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

    protected:
        bool m_isEnabled;
        //Enigma::Frameworks::CommandSubscriberPtr m_doCreatingNewTerrain;
        Enigma::Frameworks::EventSubscriberPtr m_onEditorModeChanged;
    };
}

#endif // TERRAIN_EDIT_CONSOLE_H

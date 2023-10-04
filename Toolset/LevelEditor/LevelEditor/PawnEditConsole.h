/*********************************************************************
 * \file   PawnEditConsole.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PAWN_EDIT_CONSOLE_H
#define PAWN_EDIT_CONSOLE_H

#include "Frameworks/ServiceManager.h"
#include "Frameworks/EventSubscriber.h"

namespace LevelEditor
{
    class PawnEditService;
    class PawnEditConsole : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        PawnEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<PawnEditService>& pawn_edit);
        PawnEditConsole(const PawnEditConsole&) = delete;
        PawnEditConsole& operator=(const PawnEditConsole&) = delete;
        PawnEditConsole(PawnEditConsole&&) = delete;
        PawnEditConsole& operator=(PawnEditConsole&&) = delete;
        virtual ~PawnEditConsole() override;

        virtual Enigma::Frameworks::ServiceResult OnInit() override;
        virtual Enigma::Frameworks::ServiceResult OnTerm() override;

        void InsertCandidatePawnFilePath(const std::string& pawn_name, const std::string& full_path);
        void SelectCandidatePawn(const std::string& pawn_name);

    private:
        void OnEditorModeChanged(const Enigma::Frameworks::IEventPtr& e);
        void OnSceneCursorPressed(const Enigma::Frameworks::IEventPtr& e);
        void OnSceneCursorReleased(const Enigma::Frameworks::IEventPtr& e);

    private:
        std::weak_ptr<PawnEditService> m_editService;

        bool m_isEnabled;
        Enigma::Frameworks::EventSubscriberPtr m_onEditorModeChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorPressed;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorReleased;

        typedef std::unordered_map< std::string, std::string > PawnFullpathMap;  // file base name map to full path
        PawnFullpathMap m_pawnFullpaths;
        std::string m_selectedCandidatePawnName;
    };
}

#endif // PAWN_EDIT_CONSOLE_H

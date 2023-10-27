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
#include "MathLib/Vector3.h"

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

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

        void InsertCandidatePawnFilePath(const std::string& pawn_name, const std::string& filename_at_path);
        void SelectCandidatePawn(const std::string& pawn_name);

    private:
        void OnEditorModeChanged(const Enigma::Frameworks::IEventPtr& e);
        void OnSceneCursorPressed(const Enigma::Frameworks::IEventPtr& e);
        void OnSceneCursorReleased(const Enigma::Frameworks::IEventPtr& e);

        void PutSelectedPawnToScene(const Enigma::MathLib::Vector3& pos);

    private:
        std::weak_ptr<PawnEditService> m_editService;

        bool m_isEnabled;
        Enigma::Frameworks::EventSubscriberPtr m_onEditorModeChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorPressed;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneCursorReleased;

        typedef std::unordered_map< std::string, std::string > PawnFilePathMap;  // file base name map to full path
        PawnFilePathMap m_pawnFilePaths;
        std::string m_selectedCandidatePawnName;
    };
}

#endif // PAWN_EDIT_CONSOLE_H

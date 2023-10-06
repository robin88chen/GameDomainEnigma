/*********************************************************************
 * \file   PawnEditService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PAWN_EDIT_SERVICE_H
#define PAWN_EDIT_SERVICE_H

#include "Frameworks/ServiceManager.h"
#include "MathLib/Vector3.h"
#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/SceneGraphEvents.h"
#include <system_error>
#include <deque>

namespace LevelEditor
{
    class PawnLoader;

    using error = std::error_code;

    class PawnEditService : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        PawnEditService(Enigma::Frameworks::ServiceManager* srv_mngr);
        PawnEditService(const PawnEditService&) = delete;
        PawnEditService& operator=(const PawnEditService&) = delete;
        PawnEditService(PawnEditService&&) = delete;
        PawnEditService& operator=(PawnEditService&&) = delete;
        virtual ~PawnEditService() override;

        virtual Enigma::Frameworks::ServiceResult OnInit() override;
        virtual Enigma::Frameworks::ServiceResult OnTick() override;
        virtual Enigma::Frameworks::ServiceResult OnTerm() override;

        error PutCandidatePawn(const std::string& full_path, const Enigma::MathLib::Vector3& position);

    private:
        void OnPawnLoaded(const Enigma::Frameworks::IEventPtr& e);

        void PutPawnAt(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn, const Enigma::MathLib::Vector3& position);
    private:
        PawnLoader* m_pawnLoader;

        Enigma::Frameworks::EventSubscriberPtr m_onPawnLoaded;

        struct LoadingPawnMeta
        {
            std::string m_full_path;
            Enigma::MathLib::Vector3 m_position;
        };
        std::deque<LoadingPawnMeta> m_loadingPawns;
        std::optional<LoadingPawnMeta> m_currentLoadingPawn;
    };
}

#endif // PAWN_EDIT_SERVICE_H

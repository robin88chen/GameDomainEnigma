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

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTick() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

        error putCandidatePawn(const std::string& name, const std::string& full_path, const Enigma::MathLib::Vector3& position);

    private:
        void onPrefabLoaded(const Enigma::Frameworks::IEventPtr& e);
        void onLoadPrefabFailed(const Enigma::Frameworks::IEventPtr& e);
        void onFittingNodeCreated(const Enigma::Frameworks::IEventPtr& e);
        void onCreateFittingNodeFailed(const Enigma::Frameworks::IEventPtr& e);

        void loadNextPawn();
        bool tryPutPawnAt(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn, const Enigma::MathLib::Vector3& position);
        void createFittingNodeForPawn(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn, const Enigma::MathLib::Vector3& position);

        void completePutCandidatePawn();
        void failPutCandidatePawn(const error& err);
    private:
        PawnLoader* m_pawnLoader;

        Enigma::Frameworks::EventSubscriberPtr m_onPrefabLoaded;
        Enigma::Frameworks::EventSubscriberPtr m_onLoadPrefabFailed;
        Enigma::Frameworks::EventSubscriberPtr m_onFittingNodeCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onCreateFittingNodeFailed;

        struct LoadingPawnMeta
        {
            std::string m_name;
            std::string m_full_path;
            Enigma::MathLib::Vector3 m_position;
        };
        std::deque<LoadingPawnMeta> m_loadingPawns;
        std::optional<LoadingPawnMeta> m_currentLoadingPawn;
        std::shared_ptr<Enigma::SceneGraph::Pawn> m_loadedPawn;
        Enigma::Frameworks::Ruid m_creatNodeRuid;
    };
}

#endif // PAWN_EDIT_SERVICE_H

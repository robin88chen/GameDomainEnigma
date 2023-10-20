/*********************************************************************
 * \file   PrefabIOService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PREFAB_IO_SERVICE_H
#define PREFAB_IO_SERVICE_H

#include "Frameworks/SystemService.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/DtoDeserializer.h"
#include "Frameworks/ruid.h"
#include "SceneGraph/Pawn.h"
#include <deque>
#include <system_error>

namespace Enigma::Prefabs
{
    class LoadPawnPrefab;
    using error = std::error_code;

    class PrefabIOService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        PrefabIOService(Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer);
        virtual ~PrefabIOService() override;
        PrefabIOService(const PrefabIOService&) = delete;
        PrefabIOService& operator=(const PrefabIOService&) = delete;
        PrefabIOService(PrefabIOService&&) = delete;
        PrefabIOService& operator=(PrefabIOService&&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

    private:
        void LoadNextPrefab();
        void DeserializePrefab(const Frameworks::Ruid& dto_ruid, const std::string& prefab_at_path);
        void CompletePawnPrefabLoading(const std::shared_ptr<SceneGraph::Pawn>& pawn);

        void FailPrefabLoading(error er);

        void OnDtoDeserialized(const Frameworks::IEventPtr& e);
        void OnDeserializeDtoFailed(const Frameworks::IEventPtr& e);
        void OnSceneGraphBuilt(const Frameworks::IEventPtr& e);
        void OnBuildSceneGraphFailed(const Frameworks::IEventPtr& e);

        void DoLoadingPawnPrefab(const Frameworks::ICommandPtr& c);

    private:
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;

        Frameworks::EventSubscriberPtr m_onDtoDeserialized;
        Frameworks::EventSubscriberPtr m_onDeserializeDtoFailed;
        Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
        Frameworks::EventSubscriberPtr m_onBuildSceneGraphFailed;

        Frameworks::CommandSubscriberPtr m_doLoadingPawnPrefab;

        std::deque<std::shared_ptr<LoadPawnPrefab>> m_loadingCommands;
        std::shared_ptr<LoadPawnPrefab> m_currentCommand;
    };
}

#endif // PREFAB_IO_SERVICE_H

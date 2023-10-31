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

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

    private:
        void loadNextPrefab();
        void deserializePrefab(const Frameworks::Ruid& dto_ruid, const std::string& prefab_at_path);
        void completePawnPrefabLoading(const std::shared_ptr<SceneGraph::Pawn>& pawn);

        void failPrefabLoading(error er);

        void onDtoDeserialized(const Frameworks::IEventPtr& e);
        void onDeserializeDtoFailed(const Frameworks::IEventPtr& e);
        void onSceneGraphBuilt(const Frameworks::IEventPtr& e);
        void onBuildSceneGraphFailed(const Frameworks::IEventPtr& e);
        void onPawnPrimitiveBuilt(const Frameworks::IEventPtr& e);
        void onBuildPawnPrimitiveFailed(const Frameworks::IEventPtr& e);

        void loadPawnPrefab(const Frameworks::ICommandPtr& c);

    private:
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;

        Frameworks::EventSubscriberPtr m_onDtoDeserialized;
        Frameworks::EventSubscriberPtr m_onDeserializeDtoFailed;
        Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;
        Frameworks::EventSubscriberPtr m_onBuildSceneGraphFailed;
        Frameworks::EventSubscriberPtr m_onPawnPrimitiveBuilt;
        Frameworks::EventSubscriberPtr m_onBuildPawnPrimitiveFailed;

        Frameworks::CommandSubscriberPtr m_loadPawnPrefab;

        std::deque<std::shared_ptr<LoadPawnPrefab>> m_loadingCommands;
        std::shared_ptr<LoadPawnPrefab> m_currentCommand;
        std::shared_ptr<SceneGraph::Pawn> m_loadedPawn;
    };
}

#endif // PREFAB_IO_SERVICE_H

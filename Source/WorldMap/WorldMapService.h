/*********************************************************************
 * \file   WorldMapService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_SERVICE_H
#define WORLD_MAP_SERVICE_H

#include "Frameworks/ServiceManager.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"

namespace Enigma::WorldMap
{
    class WorldMap;
    class WorldMapService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        WorldMapService(Frameworks::ServiceManager* mngr);
        WorldMapService(const WorldMapService& other) = delete;
        WorldMapService(WorldMapService&& other) noexcept = delete;
        WorldMapService& operator=(const WorldMapService& other) = delete;
        WorldMapService& operator=(WorldMapService&& other) noexcept = delete;
        virtual ~WorldMapService() override;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTerm() override;

    protected:
        void DoCreatingEmptyWorldMap(const Frameworks::ICommandPtr& c);
        void OnSceneGraphBuilt(const Frameworks::IEventPtr& e);

    protected:
        std::weak_ptr<WorldMap> m_world;

        Frameworks::CommandSubscriberPtr m_doCreatingWorldMap;
        Frameworks::EventSubscriberPtr m_onSceneGraphBuilt;  // check world map is created
    };
}

#endif // WORLD_MAP_SERVICE_H

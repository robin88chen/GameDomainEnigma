/*********************************************************************
 * \file   WorldEditService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_EDIT_SERVICE_H
#define WORLD_EDIT_SERVICE_H

#include "Frameworks/ServiceManager.h"
#include "GameEngine/GenericDto.h"
#include "Frameworks/EventSubscriber.h"
#include "WorldMap/WorldMap.h"
#include "WorldMap/WorldMapRepository.h"

namespace LevelEditor
{
    class WorldEditService : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        WorldEditService(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<Enigma::WorldMap::WorldMapRepository>& world_map_repository);
        virtual ~WorldEditService() override;

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

        //void AddNewTerrain(const std::string& terrain_name, const Enigma::Matrix4& mxLocal,
        //    const TerrainCreationSetting& terrain_creation_setting);

        std::shared_ptr<Enigma::WorldMap::WorldMap> getWorldMap() const;

        /** picking terrain
        @param clip_pos position on clipping space
        @return picked terrain and picked position on terrain */
        //std::tuple<Enigma::TerrainEntityPtr, Enigma::Vector3> PickingTerrainOnWorldMap(const Enigma::Vector2& clip_pos);

        /** serializing
        @return world map scene graph and a vector of each node graph
         */
        std::tuple<Enigma::Engine::GenericDto, std::vector<Enigma::Engine::GenericDtoCollection>> serializeWorldMapAndNodeGraphs(const std::string& path_id) const;
        void deserializeWorldMap(const Enigma::Engine::GenericDtoCollection& world_map_dto, const std::string& portal_manager_name);

    private:
        void onWorldMapCreatedOrConstituted(const Enigma::Frameworks::IEventPtr& e);
        //void OnAsyncKeyPressed(const Enigma::IMessagePtr& m);
        //void OnAsyncKeyReleased(const Enigma::IMessagePtr& m);

        //void OnEditorModeSelected(EditorModeSelectedMessage* msg);
    private:
        std::weak_ptr<Enigma::WorldMap::WorldMapRepository> m_worldMapRepository;
        std::weak_ptr<Enigma::WorldMap::WorldMap> m_worldMap;

        Enigma::Frameworks::EventSubscriberPtr m_onWorldMapCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onWorldMapConstituted;
        //std::weak_ptr<Enigma::WorldMap::WorldMapService> m_worldMap;
        //Enigma::MessageSubscriberPtr m_onAsyncKeyPressed;
        //Enigma::MessageSubscriberPtr m_onAsyncKeyReleased;

        //unsigned int m_bookedModeSelectedHandler;
    };
}

#endif // WORLD_EDIT_SERVICE_H

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
#include "WorldMap/WorldMapService.h"

namespace LevelEditor
{
    class WorldEditService : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        WorldEditService(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<Enigma::WorldMap::WorldMapService>& map);
        virtual ~WorldEditService() override;

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

        //void AddNewTerrain(const std::string& terrain_name, const Enigma::Matrix4& mxLocal,
        //    const TerrainCreationSetting& terrain_creation_setting);

        /** picking terrain
        @param clip_pos position on clipping space
        @return picked terrain and picked position on terrain */
        //std::tuple<Enigma::TerrainEntityPtr, Enigma::Vector3> PickingTerrainOnWorldMap(const Enigma::Vector2& clip_pos);

        /** serializing
        @return world map scene graph and a vector of each node graph
         */
        std::tuple<Enigma::Engine::GenericDto, std::vector<Enigma::Engine::GenericDtoCollection>> serializeWorldMapAndNodeGraphs(const std::string& path_id) const;
        void deserializeWorldMap(const Enigma::Engine::GenericDtoCollection& world_map_dto);

    private:
        //void OnAsyncKeyPressed(const Enigma::IMessagePtr& m);
        //void OnAsyncKeyReleased(const Enigma::IMessagePtr& m);

        //void OnEditorModeSelected(EditorModeSelectedMessage* msg);
    private:
        std::weak_ptr<Enigma::WorldMap::WorldMapService> m_worldMap;
        //Enigma::MessageSubscriberPtr m_onAsyncKeyPressed;
        //Enigma::MessageSubscriberPtr m_onAsyncKeyReleased;

        //unsigned int m_bookedModeSelectedHandler;
    };
}

#endif // WORLD_EDIT_SERVICE_H

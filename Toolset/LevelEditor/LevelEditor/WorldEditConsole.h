/*********************************************************************
 * \file   WorldEditConsole.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_EDIT_CONSOLE_H
#define WORLD_EDIT_CONSOLE_H

#include "Frameworks/ServiceManager.h"
#include "Frameworks/EventSubscriber.h"
#include <filesystem>

namespace LevelEditor
{
    class WorldEditService;

    class WorldEditConsole : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        WorldEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<WorldEditService>& world_service);
        virtual ~WorldEditConsole() override;

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

        void setWorldMapRootFolder(const std::filesystem::path& folder, const std::string& world_map_path_id);

        // check world files
        bool checkWorldMapFolder(const std::string& world_folder);
        // delete world files
        void deleteWorldMapFolder(const std::string& world_folder);
        // create world map directory & files
        void createWorldMapFolder(const std::string& world_folder);

        const std::string& getCurrentWorldName() const { return m_currentWorldName; }
        const std::string& getCurrentWorldFolder() const { return m_currentWorldFolder; }
        const std::string& getWorldMapPathId() const { return m_worldMapPathId; }
        //void AddNewTerrain(const std::string& terrain_name, const Enigma::Matrix4& mxLocal,
        //    const TerrainCreationSetting& terrain_creation_setting);

        /** picking terrain
        @param clip_pos position on clipping space
        @return picked terrain and picked position on terrain */
        //std::tuple<Enigma::TerrainEntityPtr, Enigma::Vector3> PickingTerrainOnWorldMap(const Enigma::Vector2& clip_pos);

        void saveWorldMap();
        void loadWorldMap(const std::filesystem::path& map_filepath);

    private:
        void onWorldMapCreated(const Enigma::Frameworks::IEventPtr& e);
        //void OnAsyncKeyPressed(const Enigma::IMessagePtr& m);
        //void OnAsyncKeyReleased(const Enigma::IMessagePtr& m);

        //void OnEditorModeSelected(EditorModeSelectedMessage* msg);
    private:
        std::filesystem::path m_mapFileRootPath;
        std::string m_currentWorldName;
        std::string m_currentWorldFolder;
        std::string m_worldMapPathId;

        std::weak_ptr<WorldEditService> m_worldEditService;

        Enigma::Frameworks::EventSubscriberPtr m_onWorldMapCreated;

        //Enigma::MessageSubscriberPtr m_onAsyncKeyPressed;
        //Enigma::MessageSubscriberPtr m_onAsyncKeyReleased;

        //unsigned int m_bookedModeSelectedHandler;
    };
}

#endif // WORLD_EDIT_CONSOLE_H

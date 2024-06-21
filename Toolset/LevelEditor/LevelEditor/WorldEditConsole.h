/*****************************************************************
 * \file   WorldEditConsole.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef WORLD_EDIT_CONSOLE_H
#define WORLD_EDIT_CONSOLE_H

#include <filesystem>
#include <SceneGraph/SceneGraph.h>

#include "Frameworks/SystemService.h"

namespace LevelEditor
{
    class WorldEditService;
    class WorldEditConsole : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        WorldEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<WorldEditService>& world_edit_service, const std::string& world_relate_path, const std::string& media_path_id);
        WorldEditConsole(const WorldEditConsole&) = delete;
        WorldEditConsole(WorldEditConsole&&) = delete;
        virtual ~WorldEditConsole() override;
        WorldEditConsole& operator=(const WorldEditConsole&) = delete;
        WorldEditConsole& operator=(WorldEditConsole&&) = delete;

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

        //! ADR: 從使用者角度，這是存檔
        void saveWorldMap();
        void loadWorldMap(const std::filesystem::path& filepath, const Enigma::SceneGraph::SpatialId& root_id);

        const std::string& worldRelativePath() const { return m_worldRelativePath; }
        const std::string& mediaPathId() const { return m_mediaPathId; }

    private:
        std::weak_ptr<WorldEditService> m_worldEditService;
        std::string m_worldRelativePath;
        std::string m_mediaPathId;
    };
}

#endif // WORLD_EDIT_CONSOLE_H

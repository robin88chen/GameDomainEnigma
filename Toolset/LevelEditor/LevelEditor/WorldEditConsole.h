/*****************************************************************
 * \file   WorldEditConsole.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef WORLD_EDIT_CONSOLE_H
#define WORLD_EDIT_CONSOLE_H

#include "Frameworks/SystemService.h"

namespace LevelEditor
{
    class WorldMapFileStoreMapper;
    class WorldEditConsole : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        WorldEditConsole(Enigma::Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<WorldMapFileStoreMapper>& world_map_file_store_mapper, const std::string& world_relate_path, const std::string& media_path_id);
        WorldEditConsole(const WorldEditConsole&) = delete;
        WorldEditConsole(WorldEditConsole&&) = delete;
        virtual ~WorldEditConsole() override;
        WorldEditConsole& operator=(const WorldEditConsole&) = delete;
        WorldEditConsole& operator=(WorldEditConsole&&) = delete;

        virtual Enigma::Frameworks::ServiceResult onInit() override;
        virtual Enigma::Frameworks::ServiceResult onTerm() override;

        bool isWorldNameDuplicated(const std::string& world_name) const;
        const std::string& worldRelativePath() const { return m_worldRelativePath; }
        const std::string& mediaPathId() const { return m_mediaPathId; }

    private:
        std::weak_ptr<WorldMapFileStoreMapper> m_worldMapFileStoreMapper;
        std::string m_worldRelativePath;
        std::string m_mediaPathId;
    };
}

#endif // WORLD_EDIT_CONSOLE_H

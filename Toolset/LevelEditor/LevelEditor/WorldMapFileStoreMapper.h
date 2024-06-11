/*****************************************************************
 * \file   WorldMapFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef EDITOR_WORLD_MAP_FILE_STORE_MAPPER_H
#define EDITOR_WORLD_MAP_FILE_STORE_MAPPER_H

#include "FileStorage/WorldMapFileStoreMapper.h"
#include "Frameworks/QuerySubscriber.h"
#include "WorldMap/WorldMapId.h"

namespace LevelEditor
{
    class WorldMapFileStoreMapper : public Enigma::FileStorage::WorldMapFileStoreMapper
    {
    public:
        WorldMapFileStoreMapper(const std::string& world_mapper_filename, const std::string& quad_root_mapper_filename, const std::shared_ptr<Enigma::Gateways::IDtoGateway>& gateway);
        ~WorldMapFileStoreMapper() override;

        void subscribeHandlers();
        void unsubscribeHandlers();

        bool isWorldNameDuplicated(const std::string& world_name);
        std::optional<Enigma::WorldMap::WorldMapId> worldMapId(const std::string& filename);

    protected:
        void isWorldNameDuplicated(const Enigma::Frameworks::IQueryPtr& q);
        void resolveWorldMapId(const Enigma::Frameworks::IQueryPtr& q);

    protected:
        Enigma::Frameworks::QuerySubscriberPtr m_isWorldNameDuplicated;
        Enigma::Frameworks::QuerySubscriberPtr m_resolveWorldMapId;
    };
}

#endif // EDITOR_WORLD_MAP_FILE_STORE_MAPPER_H

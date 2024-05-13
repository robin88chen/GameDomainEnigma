/*****************************************************************
 * \file   WorldMapFileStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef WORLD_MAP_FILE_STORE_MAPPER_H
#define WORLD_MAP_FILE_STORE_MAPPER_H

#include "WorldMap/WorldMapStoreMapper.h"
#include "WorldMap/WorldMapId.h"
#include "WorldMap/QuadTreeRootId.h"
#include "Gateways/DtoGateway.h"

namespace Enigma::FileStorage
{
    class WorldMapFileStoreMapper : public WorldMap::WorldMapStoreMapper
    {
    public:
        WorldMapFileStoreMapper(const std::string& world_mapper_filename, const std::string& quad_root_mapper_filename, const std::shared_ptr<Gateways::IDtoGateway>& gateway);
        virtual ~WorldMapFileStoreMapper() override;

        virtual std::error_code connect() override;
        virtual std::error_code disconnect() override;

        virtual bool hasQuadTreeRoot(const WorldMap::QuadTreeRootId& id) override;
        virtual std::optional<Engine::GenericDto> queryQuadTreeRoot(const WorldMap::QuadTreeRootId& id) override;
        virtual std::error_code removeQuadTreeRoot(const WorldMap::QuadTreeRootId& id) override;
        virtual std::error_code putQuadTreeRoot(const WorldMap::QuadTreeRootId& id, const Engine::GenericDto& dto) override;

        virtual bool hasWorldMap(const WorldMap::WorldMapId& id) override;
        virtual std::optional<Engine::GenericDto> queryWorldMap(const WorldMap::WorldMapId& id) override;
        virtual std::error_code removeWorldMap(const WorldMap::WorldMapId& id) override;
        virtual std::error_code putWorldMap(const WorldMap::WorldMapId& id, const Engine::GenericDto& dto) override;

    protected:
        std::error_code connectWorldMapperFile();
        std::error_code connectQuadRootMapperFile();

        std::error_code deserializeWorldMapperFile(const std::string& content);
        std::error_code deserializeQuadRootMapperFile(const std::string& content);
        std::error_code serializeWorldMapperFile();
        std::error_code serializeQuadRootMapperFile();

        Engine::GenericDto deserializeDataTransferObjects(const std::string& filename);
        std::error_code serializeDataTransferObjects(const std::string& filename, const Engine::GenericDto& dto);

        std::string extractWorldFilename(const WorldMap::WorldMapId& id, const Engine::FactoryDesc& factory_desc);
        std::string extractQuadRootFilename(const WorldMap::QuadTreeRootId& id, const Engine::FactoryDesc& factory_desc);

    protected:
        bool m_hasConnected;
        std::shared_ptr<Gateways::IDtoGateway> m_gateway;

        std::string m_worldMapperFilename;
        std::unordered_map<WorldMap::WorldMapId, std::string, WorldMap::WorldMapId::hash> m_worldFilenameMap;
        std::recursive_mutex m_worldMapLock;

        std::string m_quadRootMapperFilename;
        std::unordered_map<WorldMap::QuadTreeRootId, std::string, WorldMap::QuadTreeRootId::hash> m_quadRootFilenameMap;
        std::recursive_mutex m_quadTreeRootLock;
    };
}

#endif // WORLD_MAP_FILE_STORE_MAPPER_H

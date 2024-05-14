/*****************************************************************
 * \file   WorldMapStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef WORLD_MAP_STORE_MAPPER_H
#define WORLD_MAP_STORE_MAPPER_H

#include "QuadTreeRootId.h"
#include "QuadTreeRoot.h"
#include "WorldMapId.h"
#include "WorldMap.h"
#include <system_error>

namespace Enigma::WorldMap
{
    class WorldMapStoreMapper
    {
    public:
        virtual ~WorldMapStoreMapper() = default;

        virtual std::error_code connect() = 0;
        virtual std::error_code disconnect() = 0;

        virtual bool hasQuadTreeRoot(const QuadTreeRootId& id) = 0;
        virtual std::optional<Engine::GenericDto> queryQuadTreeRoot(const QuadTreeRootId& id) = 0;
        virtual std::error_code removeQuadTreeRoot(const QuadTreeRootId& id) = 0;
        virtual std::error_code putQuadTreeRoot(const QuadTreeRootId& id, const Engine::GenericDto& dto) = 0;

        virtual bool hasWorldMap(const WorldMapId& id) = 0;
        virtual std::optional<Engine::GenericDto> queryWorldMap(const WorldMapId& id) = 0;
        virtual std::error_code removeWorldMap(const WorldMapId& id) = 0;
        virtual std::error_code putWorldMap(const WorldMapId& id, const Engine::GenericDto& dto) = 0;
    };
}

#endif // WORLD_MAP_STORE_MAPPER_H

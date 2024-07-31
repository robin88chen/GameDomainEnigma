/*********************************************************************
 * \file   WorldMapDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 *********************************************************************/
#ifndef WORLD_MAP_DTO_H
#define WORLD_MAP_DTO_H

#include "GameEngine/GenericDto.h"
#include "GameEngine/FactoryDesc.h"
#include "WorldMapId.h"
#include "QuadTreeRootId.h"
#include "SceneGraph/SpatialId.h"
#include <vector>

namespace Enigma::WorldMap
{
    class WorldMapDto
    {
    public:
        WorldMapDto();
        WorldMapDto(const Engine::GenericDto& dto);

        [[nodiscard]] const WorldMapId& id() const { return m_id; }
        void id(const WorldMapId& id) { m_id = id; }
        [[nodiscard]] const std::optional<SceneGraph::SpatialId>& outRegionId() const { return m_outRegionId; }
        void outRegionId(const SceneGraph::SpatialId& id) { m_outRegionId = id; }
        [[nodiscard]] const std::vector<QuadTreeRootId>& quadRootIds() const { return m_quadRootIds; }
        void quadRootIds(const std::vector<QuadTreeRootId>& ids) { m_quadRootIds = ids; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        void factoryDesc(const Engine::FactoryDesc& desc) { m_factoryDesc = desc; }

        Engine::GenericDto toGenericDto() const;

    protected:
        WorldMapId m_id;
        std::optional<SceneGraph::SpatialId> m_outRegionId;
        std::vector<QuadTreeRootId> m_quadRootIds;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // WORLD_MAP_DTO_H

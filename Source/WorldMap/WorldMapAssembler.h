/*********************************************************************
 * \file   WorldMapAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef WORLD_MAP_ASSEMBLER_H
#define WORLD_MAP_ASSEMBLER_H

#include "WorldMapId.h"
#include "QuadTreeRootId.h"
#include "GameEngine/FactoryDesc.h"
#include "GameEngine/GenericDto.h"
#include "SceneGraph/SpatialId.h"

namespace Enigma::WorldMap
{
    class WorldMapAssembler
    {
    public:
        WorldMapAssembler(const WorldMapId& id);
        WorldMapAssembler(const WorldMapAssembler& other) = default;
        WorldMapAssembler(WorldMapAssembler&& other) noexcept = default;
        WorldMapAssembler& operator=(const WorldMapAssembler& other) = default;
        WorldMapAssembler& operator=(WorldMapAssembler&& other) noexcept = default;
        virtual ~WorldMapAssembler() = default;

        void outRegionId(const SceneGraph::SpatialId& id) { m_outRegionId = id; }
        void addQuadRoot(const QuadTreeRootId& id) { m_quadRootIds.push_back(id); }
        void quadRootIds(const std::vector<QuadTreeRootId>& ids) { m_quadRootIds = ids; }
        void factoryDesc(const Engine::FactoryDesc& desc) { m_factoryDesc = desc; }

        Engine::GenericDto assemble() const;

    protected:
        WorldMapId m_id;
        std::optional<SceneGraph::SpatialId> m_outRegionId;
        std::vector<QuadTreeRootId> m_quadRootIds;
        Engine::FactoryDesc m_factoryDesc;
    };

    class WorldMapDisassembler
    {
    public:
        WorldMapDisassembler();
        WorldMapDisassembler(const WorldMapDisassembler& other) = default;
        WorldMapDisassembler(WorldMapDisassembler&& other) noexcept = default;
        WorldMapDisassembler& operator=(const WorldMapDisassembler& other) = default;
        WorldMapDisassembler& operator=(WorldMapDisassembler&& other) noexcept = default;
        virtual ~WorldMapDisassembler() = default;

        void disassemble(const Engine::GenericDto& dto);

        [[nodiscard]] const WorldMapId& id() const { return m_id; }
        [[nodiscard]] const std::optional<SceneGraph::SpatialId>& outRegionId() const { return m_outRegionId; }
        [[nodiscard]] const std::vector<QuadTreeRootId>& quadRootIds() const { return m_quadRootIds; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }

    protected:
        WorldMapId m_id;
        std::optional<SceneGraph::SpatialId> m_outRegionId;
        std::vector<QuadTreeRootId> m_quadRootIds;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // WORLD_MAP_ASSEMBLER_H

/*****************************************************************
 * \file   AssetIdCombo.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 ******************************************************************/
#ifndef ASSET_ID_COMBO_H
#define ASSET_ID_COMBO_H

#include "SceneGraph/SpatialId.h"
#include "WorldMap/WorldMapId.h"
#include <optional>

namespace LevelEditor
{
    class AssetIdCombo
    {
    public:
        AssetIdCombo() = default;
        AssetIdCombo(const Enigma::SceneGraph::SpatialId& spatial_id) : m_spatialId{ spatial_id } {}
        AssetIdCombo(const Enigma::WorldMap::WorldMapId& world_map_id) : m_worldMapId{ world_map_id } {}

        bool isSpatialId() const { return m_spatialId.has_value(); }
        bool isWorldMapId() const { return m_worldMapId.has_value(); }

        Enigma::SceneGraph::SpatialId getSpatialId() const { return m_spatialId.value(); }
        Enigma::WorldMap::WorldMapId getWorldMapId() const { return m_worldMapId.value(); }

    private:
        std::optional<Enigma::SceneGraph::SpatialId> m_spatialId;
        std::optional<Enigma::WorldMap::WorldMapId> m_worldMapId;
    };
}

#endif // ASSET_ID_COMBO_H

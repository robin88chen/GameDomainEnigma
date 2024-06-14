/*********************************************************************
 * \file   LevelEditorQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 *********************************************************************/
#ifndef LEVEL_EDITOR_QUERIES_H
#define LEVEL_EDITOR_QUERIES_H

#include "WorldMap/WorldMapId.h"
#include "Frameworks/Query.h"
#include "GameEngine/TextureId.h"
#include "SceneGraph/SpatialId.h"
#include <optional>

namespace LevelEditor
{
    class ResolveTextureId : public Enigma::Frameworks::Query<std::optional<Enigma::Engine::TextureId>>
    {
    public:
        ResolveTextureId(const std::string& filename) : m_filename(filename) {}

        const std::string& filename() const { return m_filename; }

    protected:
        std::string m_filename;
    };
    class ResolveWorldId : public Enigma::Frameworks::Query<std::optional<Enigma::WorldMap::WorldMapId>>
    {
    public:
        ResolveWorldId(const std::string& filename) : m_filename(filename) {}

        const std::string& filename() const { return m_filename; }

    protected:
        std::string m_filename;
    };
    class IsWorldNameDuplicated : public Enigma::Frameworks::Query<bool>
    {
    public:
        IsWorldNameDuplicated(const std::string& world_name) : m_worldName(world_name)
        {
            m_result = false;
        }

        const std::string& worldName() const { return m_worldName; }

    protected:
        std::string m_worldName;
    };
    class QueryWorldMapIds : public Enigma::Frameworks::Query<std::vector<Enigma::WorldMap::WorldMapId>>
    {
    public:
        QueryWorldMapIds() = default;
    };
    class IsSpatialNameDuplicated : public Enigma::Frameworks::Query<bool>
    {
    public:
        IsSpatialNameDuplicated(const std::string& spatial_name) : m_spatialName(spatial_name)
        {
            m_result = false;
        }

        const std::string& spatialName() const { return m_spatialName; }

    protected:
        std::string m_spatialName;
    };
    class QueryNodeIds : public Enigma::Frameworks::Query<std::vector<Enigma::SceneGraph::SpatialId>>
    {
    public:
        QueryNodeIds() = default;
    };
}

#endif // LEVEL_EDITOR_QUERIES_H

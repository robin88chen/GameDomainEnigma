/*********************************************************************
 * \file   LevelEditorQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 *********************************************************************/
#ifndef LEVEL_EDITOR_QUERIES_H
#define LEVEL_EDITOR_QUERIES_H

#include "Frameworks/Query.h"
#include "GameEngine/TextureId.h"

namespace LevelEditor
{
    class ResolveTextureId : public Enigma::Frameworks::Query<Enigma::Engine::TextureId>
    {
    public:
        ResolveTextureId(const std::string& filename) : m_filename(filename) {}

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
}

#endif // LEVEL_EDITOR_QUERIES_H

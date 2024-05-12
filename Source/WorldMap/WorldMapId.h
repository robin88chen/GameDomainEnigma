/*********************************************************************
 * \file   WorldMapId.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 *********************************************************************/
#ifndef WORLD_MAP_ID_H
#define WORLD_MAP_ID_H

#include <string>

namespace Enigma::WorldMap
{
    class WorldMapId
    {
    public:
        WorldMapId() = default;
        WorldMapId(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

        bool operator==(const WorldMapId& other) const { return m_name == other.m_name; }
        bool operator!=(const WorldMapId& other) const { return m_name != other.m_name; }

        class hash
        {
        public:
            size_t operator()(const WorldMapId& id) const
            {
                return std::hash<std::string>()(id.m_name);
            }
        };

    protected:
        std::string m_name;
    };
}

#endif // WORLD_MAP_ID_H

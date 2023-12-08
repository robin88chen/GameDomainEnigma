/*********************************************************************
 * \file   GeometryId.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_ID_H
#define GEOMETRY_ID_H

#include <string>

namespace Enigma::Geometries
{
    class GeometryId
    {
    public:
        GeometryId() = default;
        GeometryId(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

        bool operator==(const GeometryId& other) const { return m_name == other.m_name; }
        bool operator!=(const GeometryId& other) const { return m_name != other.m_name; }

        class hash
        {
        public:
            size_t operator()(const GeometryId& id) const
            {
                return std::hash<std::string>()(id.m_name);
            }
        };
    private:
        std::string m_name;
    };
}

#endif // GEOMETRY_ID_H

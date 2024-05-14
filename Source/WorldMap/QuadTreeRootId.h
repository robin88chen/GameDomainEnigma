/*****************************************************************
 * \file   QuadTreeRootId.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef QUAD_TREE_ROOT_ID_H
#define QUAD_TREE_ROOT_ID_H

#include <string>

namespace Enigma::WorldMap
{
    class QuadTreeRootId
    {
    public:
        QuadTreeRootId() = default;
        QuadTreeRootId(const std::string& name) : m_name(name) {}

        const std::string& name() const { return m_name; }

        bool operator==(const QuadTreeRootId& other) const { return m_name == other.m_name; }
        bool operator!=(const QuadTreeRootId& other) const { return m_name != other.m_name; }

        class hash
        {
        public:
            size_t operator()(const QuadTreeRootId& id) const
            {
                return std::hash<std::string>()(id.m_name);
            }
        };
    protected:
        std::string m_name;
    };
}

#endif // QUAD_TREE_ROOT_ID_H

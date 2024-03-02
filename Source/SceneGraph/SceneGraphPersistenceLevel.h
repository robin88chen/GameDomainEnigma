/*********************************************************************
 * \file   SceneGraphPersistenceLevel.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef SCENE_GRAPH_PERSISTENCE_LEVEL_H
#define SCENE_GRAPH_PERSISTENCE_LEVEL_H

namespace Enigma::SceneGraph
{
    enum class PersistenceLevel { None, Repository, Store };
    static inline bool operator>(PersistenceLevel lhs, PersistenceLevel rhs) { return static_cast<int>(lhs) > static_cast<int>(rhs); }
    static inline bool operator>=(PersistenceLevel lhs, PersistenceLevel rhs) { return static_cast<int>(lhs) >= static_cast<int>(rhs); }
    static inline bool operator<(PersistenceLevel lhs, PersistenceLevel rhs) { return static_cast<int>(lhs) < static_cast<int>(rhs); }
    static inline bool operator<=(PersistenceLevel lhs, PersistenceLevel rhs) { return static_cast<int>(lhs) <= static_cast<int>(rhs); }
}

#endif // SCENE_GRAPH_PERSISTENCE_LEVEL_H

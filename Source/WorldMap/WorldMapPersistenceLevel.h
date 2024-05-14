/*****************************************************************
 * \file   WorldMapPersistenceLevel.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef WORLD_MAP_PERSISTENCE_LEVEL_H
#define WORLD_MAP_PERSISTENCE_LEVEL_H

namespace Enigma::WorldMap
{
    enum class PersistenceLevel { None, Repository, Store };
    static inline bool operator>(PersistenceLevel lhs, PersistenceLevel rhs) { return static_cast<int>(lhs) > static_cast<int>(rhs); }
    static inline bool operator>=(PersistenceLevel lhs, PersistenceLevel rhs) { return static_cast<int>(lhs) >= static_cast<int>(rhs); }
    static inline bool operator<(PersistenceLevel lhs, PersistenceLevel rhs) { return static_cast<int>(lhs) < static_cast<int>(rhs); }
    static inline bool operator<=(PersistenceLevel lhs, PersistenceLevel rhs) { return static_cast<int>(lhs) <= static_cast<int>(rhs); }
}

#endif // WORLD_MAP_PERSISTENCE_LEVEL_H

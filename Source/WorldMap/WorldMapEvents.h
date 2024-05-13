/*********************************************************************
 * \file   WorldMapEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_EVENTS_H
#define WORLD_MAP_EVENTS_H

#include "Frameworks/Event.h"
#include "SceneGraph/Node.h"
#include "QuadTreeRootId.h"
#include "WorldMapId.h"
#include <system_error>

namespace Enigma::WorldMap
{
    class WorldMap;
    class WorldMapPut : public Frameworks::IEvent
    {
    public:
        WorldMapPut(const WorldMapId& id) : m_id(id) {}

        const WorldMapId& id() const { return m_id; }

    protected:
        WorldMapId m_id;
    };
    class PutWorldMapFailed : public Frameworks::IEvent
    {
    public:
        PutWorldMapFailed(const WorldMapId& id, std::error_code err) : m_id(id), m_error(err) {}

        const WorldMapId& id() const { return m_id; }
        std::error_code error() const { return m_error; }

    protected:
        WorldMapId m_id;
        std::error_code m_error;
    };
    class WorldMapRemoved : public Frameworks::IEvent
    {
    public:
        WorldMapRemoved(const WorldMapId& id) : m_id(id) {}

        const WorldMapId& id() const { return m_id; }

    protected:
        WorldMapId m_id;
    };
    class RemoveWorldMapFailed : public Frameworks::IEvent
    {
    public:
        RemoveWorldMapFailed(const WorldMapId& id, std::error_code err) : m_id(id), m_error(err) {}

        const WorldMapId& id() const { return m_id; }
        std::error_code error() const { return m_error; }

    protected:
        WorldMapId m_id;
        std::error_code m_error;
    };
    class QuadTreeRootPut : public Frameworks::IEvent
    {
    public:
        QuadTreeRootPut(const QuadTreeRootId& id) : m_id(id) {}

        const QuadTreeRootId& id() const { return m_id; }

    protected:
        QuadTreeRootId m_id;
    };
    class PutQuadTreeRootFailed : public Frameworks::IEvent
    {
    public:
        PutQuadTreeRootFailed(const QuadTreeRootId& id, std::error_code err) : m_id(id), m_error(err) {}

        const QuadTreeRootId& id() const { return m_id; }
        std::error_code error() const { return m_error; }

    protected:
        QuadTreeRootId m_id;
        std::error_code m_error;
    };
    class QuadTreeRootRemoved : public Frameworks::IEvent
    {
    public:
        QuadTreeRootRemoved(const QuadTreeRootId& id) : m_id(id) {}

        const QuadTreeRootId& id() const { return m_id; }

    protected:
        QuadTreeRootId m_id;
    };
    class RemoveQuadTreeRootFailed : public Frameworks::IEvent
    {
    public:
        RemoveQuadTreeRootFailed(const QuadTreeRootId& id, std::error_code err) : m_id(id), m_error(err) {}

        const QuadTreeRootId& id() const { return m_id; }
        std::error_code error() const { return m_error; }

    protected:
        QuadTreeRootId m_id;
        std::error_code m_error;
    };
    class FittingNodeCreated : public Frameworks::IEvent
    {
    public:
        FittingNodeCreated(const QuadTreeRootId& tree_root_id) : m_quadRootId(tree_root_id) {}

        const QuadTreeRootId& quadRootId() const { return m_quadRootId; }

    protected:
        QuadTreeRootId m_quadRootId;
    };
    class FittingNodeCreationFailed : public Frameworks::IEvent
    {
    public:
        FittingNodeCreationFailed(const QuadTreeRootId& tree_root_id, std::error_code err) : m_quadRootId(tree_root_id), m_error(err) {}

        const QuadTreeRootId& quadRootId() const { return m_quadRootId; }
        std::error_code error() const { return m_error; }
    protected:
        QuadTreeRootId m_quadRootId;
        std::error_code m_error;
    };
}
#endif // WORLD_MAP_EVENTS_H

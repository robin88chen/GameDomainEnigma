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
#include <system_error>

namespace Enigma::WorldMap
{
    class WorldMap;
    class WorldMapCreated : public Frameworks::IEvent
    {
    public:
        WorldMapCreated(const std::string& name, std::shared_ptr<WorldMap> world) : m_name(name), m_world(world) {}

        const std::string& name() const { return m_name; }
        const std::shared_ptr<WorldMap>& world() const { return m_world; }

    protected:
        std::string m_name;
        std::shared_ptr<WorldMap> m_world;
    };
    class CreateWorldMapFailed : public Frameworks::IEvent
    {
    public:
        CreateWorldMapFailed(const std::string& name, std::error_code err) : m_name(name), m_error(err) {}

        const std::string& name() const { return m_name; }
        std::error_code error() const { return m_error; }

    protected:
        std::string m_name;
        std::error_code m_error;
    };
    class WorldMapDeserialized : public Frameworks::IEvent
    {
    public:
        WorldMapDeserialized(const std::string& name, std::shared_ptr<WorldMap> world) : m_name(name), m_world(world) {}

        const std::string& name() const { return m_name; }
        const std::shared_ptr<WorldMap>& world() const { return m_world; }

    protected:
        std::string m_name;
        std::shared_ptr<WorldMap> m_world;
    };
    class DeserializeWorldMapFailed : public Frameworks::IEvent
    {
    public:
        DeserializeWorldMapFailed(const std::string& name, std::error_code err) : m_name(name), m_error(err) {}

        const std::string& name() const { return m_name; }
        std::error_code error() const { return m_error; }

    protected:
        std::string m_name;
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

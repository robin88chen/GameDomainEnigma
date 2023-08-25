/*********************************************************************
 * \file   LevelEditorEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef LEVEL_EDITOR_EVENTS_H
#define LEVEL_EDITOR_EVENTS_H

#include "Frameworks/Event.h"
#include "EditorModes.h"
#include "SceneGraph/Spatial.h"

namespace LevelEditor
{
    class EditorModeChanged : public Enigma::Frameworks::IEvent
    {
    public:
        EditorModeChanged(EditorMode prev_mode, EditorMode curr_mode) : m_prevMode(prev_mode), m_currMode(curr_mode) {}

        EditorMode GetPrevMode() const { return m_prevMode; }
        EditorMode GetCurrMode() const { return m_currMode; }

    protected:
        EditorMode m_prevMode;
        EditorMode m_currMode;
    };
    class PickedSpatialChanged : public Enigma::Frameworks::IEvent
    {
    public:
        enum class PickedFrom
        {
            FromSceneGraph,
            FromSceneView
        };
    public:
        PickedSpatialChanged(const std::shared_ptr<Enigma::SceneGraph::Spatial>& spatial, PickedFrom from) : m_spatial(spatial), m_from(from) {}

        const std::shared_ptr<Enigma::SceneGraph::Spatial>& GetSpatial() const { return m_spatial; }
        PickedFrom WherePickedFrom() const { return m_from; }

    protected:
        std::shared_ptr<Enigma::SceneGraph::Spatial> m_spatial;
        PickedFrom m_from;
    };
}

#endif // LEVEL_EDITOR_EVENTS_H

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
#include "SceneGraph/Pawn.h"
#include "InputHandlers/MouseInputEvents.h"

namespace LevelEditor
{
    //---- Editor Modes ----
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
    //---- Scene Graph Events ----
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
    //---- Scene View Events ----
    class SceneCursorMoved : public Enigma::Frameworks::IEvent
    {
    public:
        SceneCursorMoved(const Enigma::MathLib::Vector3& pos, const std::shared_ptr<Enigma::SceneGraph::Pawn>& hovered_pawn, const Enigma::InputHandlers::MouseInputEvent::MouseInputParameters& mouse_param) : m_pos(pos), m_hoveredPawn(hovered_pawn), m_mouseParam(mouse_param) {}

        const Enigma::MathLib::Vector3& GetPosition() const { return m_pos; }
        std::shared_ptr<Enigma::SceneGraph::Pawn> GetHoveredPawn() const { return m_hoveredPawn.lock(); }
        const Enigma::InputHandlers::MouseInputEvent::MouseInputParameters& GetMouseParam() const { return m_mouseParam; }

    protected:
        Enigma::MathLib::Vector3 m_pos;
        std::weak_ptr<Enigma::SceneGraph::Pawn> m_hoveredPawn;
        Enigma::InputHandlers::MouseInputEvent::MouseInputParameters m_mouseParam;
    };
    class SceneCursorPressed : public Enigma::Frameworks::IEvent
    {
    public:
        SceneCursorPressed(const Enigma::MathLib::Vector3& pos, const std::shared_ptr<Enigma::SceneGraph::Pawn>& picked_pawn, const Enigma::InputHandlers::MouseInputEvent::MouseInputParameters& mouse_param) : m_pos(pos), m_pickedPawn(picked_pawn), m_mouseParam(mouse_param) {}

        const Enigma::MathLib::Vector3& GetPosition() const { return m_pos; }
        std::shared_ptr<Enigma::SceneGraph::Pawn> GetPickedPawn() const { return m_pickedPawn.lock(); }
        const Enigma::InputHandlers::MouseInputEvent::MouseInputParameters& GetMouseParam() const { return m_mouseParam; }

    protected:
        Enigma::MathLib::Vector3 m_pos;
        std::weak_ptr<Enigma::SceneGraph::Pawn> m_pickedPawn;
        Enigma::InputHandlers::MouseInputEvent::MouseInputParameters m_mouseParam;
    };
    class SceneCursorReleased : public Enigma::Frameworks::IEvent
    {
    public:
        SceneCursorReleased(const Enigma::MathLib::Vector3& pos, const std::shared_ptr<Enigma::SceneGraph::Pawn>& picked_pawn, const Enigma::InputHandlers::MouseInputEvent::MouseInputParameters& mouse_param) : m_pos(pos), m_pickedPawn(picked_pawn), m_mouseParam(mouse_param) {}

        const Enigma::MathLib::Vector3& GetPosition() const { return m_pos; }
        std::shared_ptr<Enigma::SceneGraph::Pawn> GetPickedPawn() const { return m_pickedPawn.lock(); }
        const Enigma::InputHandlers::MouseInputEvent::MouseInputParameters& GetMouseParam() const { return m_mouseParam; }

    protected:
        Enigma::MathLib::Vector3 m_pos;
        std::weak_ptr<Enigma::SceneGraph::Pawn> m_pickedPawn;
        Enigma::InputHandlers::MouseInputEvent::MouseInputParameters m_mouseParam;
    };
    class SceneCursorDragged : public Enigma::Frameworks::IEvent
    {
    public:
        SceneCursorDragged(const Enigma::MathLib::Vector3& pos, const std::shared_ptr<Enigma::SceneGraph::Pawn>& hovered_pawn, const Enigma::InputHandlers::MouseInputEvent::MouseInputParameters& mouse_param) : m_pos(pos), m_hoveredPawn(hovered_pawn), m_mouseParam(mouse_param) {}

        const Enigma::MathLib::Vector3& GetPosition() const { return m_pos; }
        std::shared_ptr<Enigma::SceneGraph::Pawn> GetHoveredPawn() const { return m_hoveredPawn.lock(); }
        const Enigma::InputHandlers::MouseInputEvent::MouseInputParameters& GetMouseParam() const { return m_mouseParam; }

    protected:
        Enigma::MathLib::Vector3 m_pos;
        std::weak_ptr<Enigma::SceneGraph::Pawn> m_hoveredPawn;
        Enigma::InputHandlers::MouseInputEvent::MouseInputParameters m_mouseParam;
    };
    //---- Pawn Events ----
    class PawnLoaded : public Enigma::Frameworks::IEvent
    {
    public:
        PawnLoaded(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn, const std::string& pawn_file_path) : m_pawn(pawn), m_pawnFilePath(pawn_file_path) {}

        std::shared_ptr<Enigma::SceneGraph::Pawn> GetPawn() const { return m_pawn; }
        const std::string& GetPawnFilePath() const { return m_pawnFilePath; }

    private:
        std::shared_ptr<Enigma::SceneGraph::Pawn> m_pawn;
        std::string m_pawnFilePath;
    };
}

#endif // LEVEL_EDITOR_EVENTS_H

/*********************************************************************
 * \file   EditorSceneConsole.h
 * \brief  處理 editor scene view 中的操作
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef EDITOR_SCENE_CONSOLE_H
#define EDITOR_SCENE_CONSOLE_H

#include "Frameworks/ServiceManager.h"
#include "Frameworks/EventSubscriber.h"
#include "GraphicKernel/TargetViewPort.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/Node.h"

namespace LevelEditor
{
    class EditorSceneConsole : public Enigma::Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        EditorSceneConsole(Enigma::Frameworks::ServiceManager* srv_mngr);
        EditorSceneConsole(const EditorSceneConsole&) = delete;
        EditorSceneConsole& operator=(const EditorSceneConsole&) = delete;
        virtual ~EditorSceneConsole() override;
        EditorSceneConsole(EditorSceneConsole&&) = delete;
        EditorSceneConsole& operator=(EditorSceneConsole&&) = delete;

        virtual Enigma::Frameworks::ServiceResult OnInit() override;
        virtual Enigma::Frameworks::ServiceResult OnTerm() override;

    protected:
        void OnGameCameraCreated(const Enigma::Frameworks::IEventPtr& e);
        void OnSceneRootCreated(const Enigma::Frameworks::IEventPtr& e);
        void OnTargetViewportChanged(const Enigma::Frameworks::IEventPtr& e);
        void OnMouseMoved(const Enigma::Frameworks::IEventPtr& e);

        std::tuple<std::shared_ptr<Enigma::SceneGraph::Pawn>, Enigma::MathLib::Vector3> PickingOnSceneView(const Enigma::MathLib::Vector2& clip_pos);

    protected:
        Enigma::Frameworks::EventSubscriberPtr m_onGameCameraCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onSceneRootCreated;
        Enigma::Frameworks::EventSubscriberPtr m_onTargetViewportChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onMouseMoved;

        std::weak_ptr<Enigma::SceneGraph::Camera> m_camera;
        std::weak_ptr<Enigma::SceneGraph::Node> m_sceneRoot;
        Enigma::Graphics::TargetViewPort m_targetViewport;
    };
}
#endif // EDITOR_SCENE_CONSOLE_H

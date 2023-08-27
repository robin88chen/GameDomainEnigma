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
        void OnTargetViewportChanged(const Enigma::Frameworks::IEventPtr& e);
        void OnMouseMoved(const Enigma::Frameworks::IEventPtr& e);

    protected:
        Enigma::Frameworks::EventSubscriberPtr m_onTargetViewportChanged;
        Enigma::Frameworks::EventSubscriberPtr m_onMouseMoved;

        Enigma::Graphics::TargetViewPort m_targetViewport;
    };
}
#endif // EDITOR_SCENE_CONSOLE_H

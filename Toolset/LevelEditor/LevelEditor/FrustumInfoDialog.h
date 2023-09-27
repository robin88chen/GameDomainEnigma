/*********************************************************************
 * \file   FrustumInfoDialog.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2023
 *********************************************************************/
#ifndef FRUSTUM_INFO_DIALOG_H
#define FRUSTUM_INFO_DIALOG_H

#include "Frameworks/EventSubscriber.h"
#include "SceneGraph/Camera.h"
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>

namespace LevelEditor
{
    class FrustumInfoDialog : public nana::form
    {
    public:
        FrustumInfoDialog(nana::window owner, const std::string& camera_name);
        virtual ~FrustumInfoDialog();

        void RegisterHandlers();
        void UnregisterHandlers();

        void OnOkButton(const nana::arg_click& arg);
        void OnCancelButton(const nana::arg_click& arg);

    private:
        void OnReplyCameraQuery(const Enigma::Frameworks::IEventPtr& e);

    private:
        nana::button* m_okButton;
        nana::button* m_cancelButton;

        nana::label* m_fovPrompt;
        nana::textbox* m_fovInputBox;
        nana::label* m_nearPlanePrompt;
        nana::textbox* m_nearPlaneInputBox;
        nana::label* m_farPlanePrompt;
        nana::textbox* m_farPlaneInputBox;

        std::weak_ptr<Enigma::SceneGraph::Camera> m_camera;

        Enigma::Frameworks::EventSubscriberPtr m_onReplyCameraQuery;
        Enigma::Frameworks::EventSubscriberPtr m_onQueryCameraFailed;
    };
}

#endif // FRUSTUM_INFO_DIALOG_H


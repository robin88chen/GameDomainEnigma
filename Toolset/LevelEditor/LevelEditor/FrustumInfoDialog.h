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
        FrustumInfoDialog(nana::window owner, const Enigma::SceneGraph::SpatialId& camera_id);
        virtual ~FrustumInfoDialog();

        void registerHandlers();
        void unregisterHandlers();

        void onOkButton(const nana::arg_click& arg);
        void onCancelButton(const nana::arg_click& arg);

    private:
        void queryCamera(const Enigma::SceneGraph::SpatialId& camera_id);

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
    };
}

#endif // FRUSTUM_INFO_DIALOG_H


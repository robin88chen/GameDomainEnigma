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
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/textbox.hpp>

namespace LevelEditor
{
    class FrustumInfoDialog : public nana::form
    {
    public:
        FrustumInfoDialog(nana::window owner, const std::string& frustum_name);
        virtual ~FrustumInfoDialog();

        void RegisterHandlers();
        void UnregisterHandlers();

        void OnOkButton(const nana::arg_click& arg);
        void OnCancelButton(const nana::arg_click& arg);

    private:
        void OnReplyFrustumQuery(const Enigma::Frameworks::IEventPtr& e);

    private:
        nana::button* m_okButton;
        nana::button* m_cancelButton;

        nana::label* m_fovPrompt;
        nana::textbox* m_fovInputBox;
        nana::label* m_nearPlanePrompt;
        nana::textbox* m_nearPlaneInputBox;
        nana::label* m_farPlanePrompt;
        nana::textbox* m_farPlaneInputBox;

        Enigma::Frameworks::EventSubscriberPtr m_onReplyFrustumQuery;
        Enigma::Frameworks::EventSubscriberPtr m_onQueryFrustumFailed;
    };
}

#endif // FRUSTUM_INFO_DIALOG_H


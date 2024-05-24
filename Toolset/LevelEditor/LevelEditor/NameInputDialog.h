/*****************************************************************
 * \file   NameInputDialog.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2024
 ******************************************************************/
#ifndef NAME_INPUT_DIALOG_H
#define NAME_INPUT_DIALOG_H

#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/label.hpp>

namespace LevelEditor
{
    class NameInputDialog : public nana::form
    {
    public:
        enum class Answer
        {
            ok,
            cancel
        };
    public:
        NameInputDialog(nana::window owner, const std::string& prompt, const std::function<bool(const std::string&)>& duplicate_name_verifier);
        virtual ~NameInputDialog();

        Answer modalityShow();
        const std::string& getInputName() const { return m_inputName; }

    private:
        void onOkButton(const nana::arg_click& arg);
        void onCancelButton(const nana::arg_click& arg);
        void onInputNameChanged(const nana::arg_textbox& arg);

    private:
        std::function<bool(const std::string&)> m_duplicateNameVerifier;
        nana::button* m_okButton;
        nana::button* m_cancelButton;
        nana::label* m_namePrompt;
        nana::textbox* m_nameInputBox;
        nana::label* m_nameValidationPrompt;
        Answer m_answer;
        std::string m_inputName;
    };
}

#endif // NAME_INPUT_DIALOG_H

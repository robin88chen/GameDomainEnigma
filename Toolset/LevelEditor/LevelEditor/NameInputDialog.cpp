#include "NameInputDialog.h"
#include "Platforms/MemoryMacro.h"

using namespace LevelEditor;

NameInputDialog::NameInputDialog(nana::window owner, const std::string& prompt, const std::function<bool(const std::string&)>& duplicate_name_verifier) : form(owner, nana::API::make_center(320, 128), nana::appear::decorate<>{})
{
    m_duplicateNameVerifier = duplicate_name_verifier;
    m_answer = Answer::cancel;
    caption(prompt);
    get_place().div("vert<create_prompt arrange=[30%,variable] margin=[10,20]><validation_prompt margin=[10,20]><buttons margin=[10,20] gap=10>");
    m_namePrompt = menew nana::label(*this, "Name : ");
    (*m_namePrompt).text_align(nana::align::right);
    m_nameInputBox = menew nana::textbox(*this, "");
    m_nameInputBox->events().text_changed([this](const nana::arg_textbox& a) { this->onInputNameChanged(a); });
    get_place()["create_prompt"] << *m_namePrompt << *m_nameInputBox;
    m_nameValidationPrompt = menew nana::label(*this, "");
    (*m_nameValidationPrompt).text_align(nana::align::right);
    get_place()["validation_prompt"] << *m_nameValidationPrompt;
    m_okButton = menew nana::button(*this, "OK");
    m_okButton->events().click([this](const nana::arg_click& a) { this->onOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->onCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    get_place().collocate();
}

NameInputDialog::~NameInputDialog()
{
    SAFE_DELETE(m_namePrompt);
    SAFE_DELETE(m_nameInputBox);
    SAFE_DELETE(m_nameValidationPrompt);
    SAFE_DELETE(m_okButton);
    SAFE_DELETE(m_cancelButton);
}

void NameInputDialog::onOkButton(const nana::arg_click& arg)
{
    m_answer = Answer::ok;
    close();
}

void NameInputDialog::onCancelButton(const nana::arg_click& arg)
{
    m_answer = Answer::cancel;
    close();
}

void NameInputDialog::onInputNameChanged(const nana::arg_textbox& arg)
{
    if (arg.widget.text().empty())
    {
        m_nameValidationPrompt->caption("world name empty");
    }
    else if (m_duplicateNameVerifier(arg.widget.text()))
    {
        m_nameValidationPrompt->caption("duplicated name");
    }
    else
    {
        m_nameValidationPrompt->caption("");
    }
}

NameInputDialog::Answer NameInputDialog::modalizeShow()
{
    modality();
    show();
    return m_answer;
}


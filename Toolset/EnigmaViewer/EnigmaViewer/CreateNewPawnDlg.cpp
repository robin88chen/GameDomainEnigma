#include "CreateNewPawnDlg.h"
#include "Platforms/MemoryMacro.h"
#include "ViewerQueries.h"

using namespace EnigmaViewer;

CreateNewPawnDlg::CreateNewPawnDlg(nana::window owner) : form(owner, nana::API::make_center(400, 320), nana::appear::decorate<>{})
{
    caption("Create New Pawn");
    get_place().div("vert<><create_prompt arrange=[40%,variable] margin=[10,20]><model_select_prompt arrange=[40%,variable] margin=[10,20]><><buttons margin=[10,40] gap=10><>");
    m_namePrompt = menew nana::label(*this, "Animated Pawn Name : ");
    (*m_namePrompt).text_align(nana::align::right);
    m_nameInputBox = menew nana::textbox(*this, "");
    get_place()["create_prompt"] << *m_namePrompt << *m_nameInputBox;
    m_modelSelectPrompt = menew nana::label(*this, "Model Select : ");
    (*m_modelSelectPrompt).text_align(nana::align::right);
    m_modelSelectBox = menew nana::combox(*this);
    auto model_list = std::make_shared<RequestModelNames>()->dispatch();
    for (const auto& model_name : model_list)
    {
        m_modelSelectBox->push_back(model_name);
    }
    get_place()["model_select_prompt"] << *m_modelSelectPrompt << *m_modelSelectBox;
    m_okButton = menew nana::button(*this, "OK");
    m_okButton->events().click([this](const nana::arg_click& a) { this->onOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->onCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    get_place().collocate();
}

CreateNewPawnDlg::~CreateNewPawnDlg()
{
    SAFE_DELETE(m_namePrompt);
    SAFE_DELETE(m_nameInputBox);
    SAFE_DELETE(m_modelSelectPrompt);
    SAFE_DELETE(m_modelSelectBox);
    SAFE_DELETE(m_okButton);
    SAFE_DELETE(m_cancelButton);
}

void CreateNewPawnDlg::onOkButton(const nana::arg_click& arg)
{
    close();
}

void CreateNewPawnDlg::onCancelButton(const nana::arg_click& arg)
{
    close();
}

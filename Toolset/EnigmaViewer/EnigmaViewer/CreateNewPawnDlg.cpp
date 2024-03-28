#include "CreateNewPawnDlg.h"
#include "Platforms/MemoryMacro.h"

using namespace EnigmaViewer;

CreateNewPawnDlg::CreateNewPawnDlg(nana::window owner) : form(owner, nana::API::make_center(400, 320), nana::appear::decorate<>{})
{
    caption("Create New Pawn");
    get_place().div("vert<><create_prompt arrange=[40%,variable] margin=[10,20]><folder_name_prompt arrange=[40%,variable] margin=[10,20]><delete_folder_prompt margin=[10,20]><><buttons margin=[10,40] gap=10><>");
    m_okButton = menew nana::button(*this, "OK");
    m_okButton->events().click([this](const nana::arg_click& a) { this->onOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->onCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    get_place().collocate();
}

CreateNewPawnDlg::~CreateNewPawnDlg()
{
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

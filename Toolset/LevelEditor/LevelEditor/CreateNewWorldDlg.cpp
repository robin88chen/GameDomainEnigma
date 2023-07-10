#include "CreateNewWorldDlg.h"
#include "Frameworks/CommandBus.h"
#include "WorldMap/WorldMapCommands.h"
#include "WorldMap/WorldMapDto.h"
#include "Platforms/PlatformLayer.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/MemoryAllocMacro.h"
#include "WorldEditConsole.h"
#include "nana/gui.hpp"

using namespace LevelEditor;
using namespace Enigma;

CreateNewWorldDlg::CreateNewWorldDlg(nana::window owner, const std::shared_ptr<WorldEditConsole>& world_editor) : form(owner, nana::API::make_center(400, 240), nana::appear::decorate<>{})
{
    m_worldEditor = world_editor;
    caption("Create New World");
    get_place().div("vert<><create_prompt arrange=[40%,variable] margin=[10,20]><node_name_prompt arrange=[40%,variable] margin=[10,20]><><buttons margin=[10,40] gap=10><>");
    m_namePrompt = menew nana::label(*this, "World Map Name : ");
    (*m_namePrompt).text_align(nana::align::right);
    m_nameInputBox = menew nana::textbox(*this, "");
    get_place()["create_prompt"] << *m_namePrompt << *m_nameInputBox;
    //m_rootNodePrompt = menew nana::label(*this, "Map Root Node Name : ");
    //(*m_rootNodePrompt).text_align(nana::align::right);
    //m_rootNodeNameInputBox = menew nana::textbox(*this, "");
    //get_place()["node_name_prompt"] << *m_rootNodePrompt << *m_rootNodeNameInputBox;
    m_okButton = menew nana::button(*this, "OK");
    m_okButton->events().click([this](const nana::arg_click& a) { this->OnOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->OnCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    get_place().collocate();
}

CreateNewWorldDlg::~CreateNewWorldDlg()
{
    SAFE_DELETE(m_namePrompt);
    SAFE_DELETE(m_nameInputBox);
    //SAFE_DELETE(m_rootNodePrompt);
    //SAFE_DELETE(m_rootNodeNameInputBox);
    SAFE_DELETE(m_okButton);
    SAFE_DELETE(m_cancelButton);
}

void CreateNewWorldDlg::OnOkButton(const nana::arg_click& arg)
{
    if (FATAL_LOG_EXPR(m_worldEditor.expired())) return;
    if (const bool world_file_exist = m_worldEditor.lock()->CheckWorldMapFiles(m_nameInputBox->text()); world_file_exist)
    {
        nana::msgbox mb(*this, "Create New World Map", nana::msgbox::yes_no);
        mb << "Delete Exist World Map Files?";
        auto answer = mb.show();
        if (answer == nana::msgbox::pick_no)
        {
            close();
        }
        else
        {
            m_worldEditor.lock()->DeleteWorldMapFiles(m_nameInputBox->text());
        }
    }

    std::string world_name = m_nameInputBox->text();
    //std::string root_node_name = world_name + "." + m_rootNodeNameInputBox->text();
    WorldMap::WorldMapDto world_map_dto;
    world_map_dto.Name() = world_name;
    world_map_dto.IsTopLevel() = true;
    Frameworks::CommandBus::Post(std::make_shared<WorldMap::CreateEmptyWorldMap>(world_map_dto.ToGenericDto()));
    m_worldEditor.lock()->CreateWorldMapFiles(world_name);

    close();
}

void CreateNewWorldDlg::OnCancelButton(const nana::arg_click& arg)
{
    close();
}

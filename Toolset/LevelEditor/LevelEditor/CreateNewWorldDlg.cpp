#include "CreateNewWorldDlg.h"

#include <WorldMap/WorldMap.h>

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

CreateNewWorldDlg::CreateNewWorldDlg(nana::window owner, const std::shared_ptr<WorldEditConsole>& world_editor, const std::string& portal_manager_name) : form(owner, nana::API::make_center(400, 320), nana::appear::decorate<>{})
{
    m_portalManagerName = portal_manager_name;
    m_worldEditor = world_editor;
    caption("Create New World");
    get_place().div("vert<><create_prompt arrange=[40%,variable] margin=[10,20]><folder_name_prompt arrange=[40%,variable] margin=[10,20]><delete_folder_prompt margin=[10,20]><><buttons margin=[10,40] gap=10><>");
    m_namePrompt = menew nana::label(*this, "World Map Name : ");
    (*m_namePrompt).text_align(nana::align::right);
    m_nameInputBox = menew nana::textbox(*this, "");
    get_place()["create_prompt"] << *m_namePrompt << *m_nameInputBox;
    m_folderNamePrompt = menew nana::label(*this, "World Folder Name : ");
    (*m_folderNamePrompt).text_align(nana::align::right);
    m_folderNameInputBox = menew nana::textbox(*this, "");
    get_place()["folder_name_prompt"] << *m_folderNamePrompt << *m_folderNameInputBox;
    m_okButton = menew nana::button(*this, "OK");
    m_okButton->events().click([this](const nana::arg_click& a) { this->onOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->onCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    m_deleteExistFolderCheckBox = menew nana::checkbox(*this, "Delete Exist Folder");
    get_place()["delete_folder_prompt"] << *m_deleteExistFolderCheckBox;
    get_place().collocate();
}

CreateNewWorldDlg::~CreateNewWorldDlg()
{
    SAFE_DELETE(m_namePrompt);
    SAFE_DELETE(m_nameInputBox);
    SAFE_DELETE(m_folderNamePrompt);
    SAFE_DELETE(m_folderNameInputBox);
    SAFE_DELETE(m_okButton);
    SAFE_DELETE(m_cancelButton);
    SAFE_DELETE(m_deleteExistFolderCheckBox);
}

void CreateNewWorldDlg::onOkButton(const nana::arg_click& arg)
{
    if (FATAL_LOG_EXPR(m_worldEditor.expired())) return;
    std::string folder_name = m_folderNameInputBox->text();
    if (m_deleteExistFolderCheckBox->checked())
    {
        if (const bool world_file_exist = m_worldEditor.lock()->checkWorldMapFolder(folder_name); world_file_exist)
        {
            nana::msgbox mb(*this, "Create New World Map", nana::msgbox::yes_no);
            mb << "Delete Exist World Map Folder?";
            auto answer = mb.show();
            if (answer == nana::msgbox::pick_yes)
            {
                m_worldEditor.lock()->deleteWorldMapFolder(folder_name);
            }
        }
    }

    std::string world_name = m_nameInputBox->text();
    Engine::FactoryDesc factory_desc(WorldMap::WorldMap::TYPE_RTTI.getName());
    factory_desc.ClaimAsInstanced(folder_name + "/" + world_name + ".wld");
    /*SceneGraph::PortalZoneNodeDto portal_root_dto;
    portal_root_dto.Name() = world_name + "_portal_root";
    WorldMap::WorldMapDto world_map_dto;
    world_map_dto.name() = world_name;
    world_map_dto.factoryDesc().ClaimAsInstanced(folder_name + "/" + world_name + ".wld");
    world_map_dto.portalRoot() = portal_root_dto.toGenericDto();*/
    Frameworks::CommandBus::post(std::make_shared<WorldMap::CreateEmptyWorldMap>(world_name, factory_desc, m_portalManagerName));
    m_worldEditor.lock()->createWorldMapFolder(folder_name);

    close();
}

void CreateNewWorldDlg::onCancelButton(const nana::arg_click& arg)
{
    close();
}

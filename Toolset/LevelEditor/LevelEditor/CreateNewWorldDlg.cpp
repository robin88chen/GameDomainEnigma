#include "CreateNewWorldDlg.h"
#include "LevelEditorCommands.h"
#include "LevelEditorErrors.h"
#include "WorldMap/WorldMap.h"
#include "Frameworks/CommandBus.h"
#include "WorldMap/WorldMapQueries.h"
#include "WorldMap/WorldMapDto.h"
#include "WorldMap/WorldMapCommands.h"
#include "Platforms/PlatformLayer.h"
#include "Platforms/MemoryMacro.h"
#include "WorldEditConsole.h"
#include "SceneGraph/PortalZoneNode.h"
#include "SceneGraph/SceneGraphAssemblers.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "GameCommon/GameSceneCommands.h"
#include "InputNameVerifier.h"
#include "nana/gui.hpp"

using namespace LevelEditor;
using namespace Enigma;

CreateNewWorldDlg::CreateNewWorldDlg(nana::window owner, const std::shared_ptr<WorldEditConsole>& world_editor, const SceneGraph::SpatialId& portal_manager_id) : form(owner, nana::API::make_center(480, 320), nana::appear::decorate<>{})
{
    m_portalManagerId = portal_manager_id;
    m_worldEditor = world_editor;
    caption("Create New World");
    get_place().div(
        "vert<><create_prompt arrange=[45%,variable] margin=[10,20]>"
        "<region_prompt arrange=[45%,variable] margin=[10,20]>"
        "<validation_prompt arrange=[45%,variable] margin=[10,20]>"
        "<><buttons margin=[10,40] gap=10><>");
    std::string prompt = "World Map Name : " + world_editor->worldRelativePath() + "/";
    m_namePrompt = menew nana::label(*this, prompt);
    (*m_namePrompt).text_align(nana::align::right);
    m_nameInputBox = menew nana::textbox(*this, "");
    m_nameInputBox->events().text_changed([this](const nana::arg_textbox& a) { this->onInputNameChanged(a); });
    get_place()["create_prompt"] << *m_namePrompt << *m_nameInputBox;
    std::string region_prompt = "Outside Region : " + world_editor->worldRelativePath() + "/";
    m_regionCheckbox = menew nana::checkbox(*this, region_prompt);
    m_regionNameInputBox = menew nana::textbox(*this, "");
    m_regionNameInputBox->events().text_changed([this](const nana::arg_textbox& a) { this->onInputNameChanged(a); });
    get_place()["region_prompt"] << *m_regionCheckbox << *m_regionNameInputBox;
    m_nameValidationPrompt = menew nana::label(*this, "world name empty");
    (*m_nameValidationPrompt).text_align(nana::align::right);
    get_place()["validation_prompt"] << *m_nameValidationPrompt;
    m_okButton = menew nana::button(*this, "OK");
    m_okButton->events().click([this](const nana::arg_click& a) { this->onOkButton(a); });
    m_cancelButton = menew nana::button(*this, "Cancel");
    m_cancelButton->events().click([this](const nana::arg_click& a) { this->onCancelButton(a); });
    get_place()["buttons"] << *m_okButton << *m_cancelButton;
    get_place().collocate();
}

CreateNewWorldDlg::~CreateNewWorldDlg()
{
    SAFE_DELETE(m_namePrompt);
    SAFE_DELETE(m_nameInputBox);
    SAFE_DELETE(m_regionCheckbox);
    SAFE_DELETE(m_regionNameInputBox);
    SAFE_DELETE(m_nameValidationPrompt);
    SAFE_DELETE(m_okButton);
    SAFE_DELETE(m_cancelButton);
}

void CreateNewWorldDlg::onOkButton(const nana::arg_click& arg)
{
    if (FATAL_LOG_EXPR(m_worldEditor.expired())) return;
    std::string world_name = m_nameInputBox->text();
    if (auto er = validateInputNames())
    {
        Frameworks::CommandBus::enqueue(std::make_shared<OutputMessage>(er.message()));
        return;
    }

    auto world_id = WorldMap::WorldMapId(m_worldEditor.lock()->worldRelativePath() + "/" + world_name);
    std::optional<SceneGraph::SpatialId> region_id;
    if (m_regionCheckbox->checked())
    {
        region_id = outsideRegionFromInput();
        Engine::FactoryDesc factory_desc(region_id->rtti());
        factory_desc.ClaimAsDeferred(region_id->name() + ".node", m_worldEditor.lock()->mediaPathId());
        std::make_shared<WorldMap::CreateWorldMapOutsideRegion>(*region_id, factory_desc)->execute();
    }
    Engine::FactoryDesc factory_desc(WorldMap::WorldMap::TYPE_RTTI);
    factory_desc.ClaimAsInstanced(world_id.name() + ".wld", m_worldEditor.lock()->mediaPathId());
    std::make_shared<WorldMap::CreateEmptyWorldMap>(world_id, factory_desc, region_id)->execute();

    close();
}

void CreateNewWorldDlg::onCancelButton(const nana::arg_click& arg)
{
    close();
}

void CreateNewWorldDlg::onInputNameChanged(const nana::arg_textbox& arg)
{
    if (FATAL_LOG_EXPR(m_worldEditor.expired())) return;
    std::error_code er = validateInputNames();
    if (er)
    {
        m_nameValidationPrompt->caption(er.message());
    }
    else
    {
        m_nameValidationPrompt->caption("");
    }
}

Enigma::SceneGraph::SpatialId CreateNewWorldDlg::outsideRegionFromInput()
{
    assert(!m_worldEditor.expired());
    std::string region_name = m_regionNameInputBox->text();
    SceneGraph::SpatialId region_id = SceneGraph::SpatialId(m_worldEditor.lock()->worldRelativePath() + "/" + region_name, SceneGraph::PortalZoneNode::TYPE_RTTI);
    return region_id;
}

std::error_code CreateNewWorldDlg::validateInputNames()
{
    assert(!m_worldEditor.expired());
    std::string world_path_name = m_worldEditor.lock()->worldRelativePath() + "/" + m_nameInputBox->text();
    std::string region_path_name = m_worldEditor.lock()->worldRelativePath() + "/" + m_regionNameInputBox->text();
    if (m_nameInputBox->text().empty())
    {
        return ErrorCode::emptyWorldName;
    }
    else if (InputNameVerifier::isWorldNameDuplicated(world_path_name))
    {
        return ErrorCode::duplicatedWorldName;
    }
    else if (m_regionCheckbox->checked() && m_regionNameInputBox->text().empty())
    {
        return ErrorCode::emptyOutRegionName;
    }
    else if (m_regionCheckbox->checked() && InputNameVerifier::isSpatialNameDuplicated(region_path_name))
    {
        return ErrorCode::duplicatedOutRegionName;
    }
    return ErrorCode::ok;
}
